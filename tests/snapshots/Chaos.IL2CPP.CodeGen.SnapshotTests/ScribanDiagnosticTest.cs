// Diagnostic: find Scriban-compatible for-loop data structures
using System;
using System.Collections.Generic;
using Scriban;
using Scriban.Runtime;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.SnapshotTests;

public sealed class ScribanDiagnosticTest
{
    private static string RenderFor(string template, string key, object value)
    {
        var model = new ScriptObject { [key] = value };
        var t = Template.ParseLiquid(template);
        Assert.False(t.HasErrors, string.Join("; ", t.Messages));
        var ctx = new TemplateContext();
        ctx.LoopLimit = 100000;
        ctx.LimitToString = -1;  // matches ScribanTemplateRenderer.RenderTemplate
        ctx.PushGlobal(new ScriptObject { ["tab"] = "\t" });
        ctx.PushGlobal(model);
        try { return t.Render(ctx).TrimEnd(); }
        finally { ctx.PopGlobal(); }
    }

    [Fact] public void IntArray() =>
        Assert.Equal("42", RenderFor("{% for x in items %}{{ x }}{% endfor %}", "items", new int[] { 42 }));

    [Fact] public void IntList() =>
        Assert.Equal("42", RenderFor("{% for x in items %}{{ x }}{% endfor %}", "items", new List<int> { 42 }));

    [Fact] public void StringArray() =>
        Assert.Equal("hi", RenderFor("{% for x in items %}{{ x }}{% endfor %}", "items", new string[] { "hi" }));

    [Fact] public void ScriptObjectArray_Works() =>
        Assert.Equal("1", RenderFor("{% for m in items %}{{ m.x }}{% endfor %}",
            "items", new ScriptObject[] { new() { ["x"] = 1 } }));

    [Fact] public void ScriptObjectList_Works() =>
        Assert.Equal("1", RenderFor("{% for m in items %}{{ m.x }}{% endfor %}",
            "items", new List<ScriptObject> { new() { ["x"] = 1 } }));

    [Fact] public void ListOfObject_Works() =>
        Assert.Equal("1", RenderFor("{% for m in items %}{{ m.x }}{% endfor %}",
            "items", new List<object> { new ScriptObject { ["x"] = 1 } }));

    [Fact] public void ListOfDictionary_Works() =>
        Assert.Equal("1", RenderFor("{% for m in items %}{{ m.x }}{% endfor %}",
            "items", new List<Dictionary<string, object>> {
                new() { ["x"] = 1 }
            }));

    [Fact] public void ScriptObject_WithIndexer() =>
        Assert.Equal("42", RenderFor("{% for i in (1..1) %}{{ items[i] }}{% endfor %}",
            "items", new ScriptObject { ["1"] = 42 }));

    // ── Non-Liquid mode tests (uses Template.Parse, matches real rendering path) ──

    private static string RenderNonLiquid(string template, string key, object value)
    {
        var model = new ScriptObject { [key] = value };
        var t = Template.Parse(template);
        Assert.False(t.HasErrors, string.Join("; ", t.Messages));
        var ctx = new TemplateContext();
        ctx.LoopLimit = 100000;
        ctx.LimitToString = -1;
        ctx.PushGlobal(new ScriptObject { ["tab"] = "\t" });
        ctx.PushGlobal(model);
        try { return t.Render(ctx).TrimEnd(); }
        finally { ctx.PopGlobal(); }
    }

    [Fact]
    public void NonLiquid_ForEach_OverListOfScriptObject()
    {
        var result = RenderNonLiquid(
            "{{~ for m in items ~}}{{ m.x }}{{~ end ~}}",
            "items", new List<ScriptObject> { new() { ["x"] = 42 } });
        Assert.Equal("42", result);
    }

    [Fact]
    public void NonLiquid_NotOperator_WithForLast()
    {
        var items = new List<ScriptObject>
        {
            new() { ["x"] = 1 },
            new() { ["x"] = 2 },
        };
        var result = RenderNonLiquid(
            "{{~ for m in items ~}}{{ m.x }}{{~ if !for.last ~}}, {{~ end ~}}{{~ end ~}}",
            "items", items);
        Assert.Equal("1,2", result);
    }

    [Fact]
    public void NonLiquid_AndOperator_WithMemberExpression()
    {
        var items = new List<ScriptObject>
        {
            new() { ["is_string"] = true, ["id"] = 42 },
            new() { ["is_string"] = false, ["id"] = 0 },
        };
        var result = RenderNonLiquid(
            "{{~ for m in items ~}}{{- if m.is_string && m.id != 0 -}}yes{{- else -}}no{{- end -}}{{- end -}}",
            "items", items);
        Assert.Equal("yesno", result);
    }

    [Fact]
    public void NonLiquid_CombinedInline_NestedForIf()
    {
        // Replicates the complex inline pattern from DispatchEntryCode template
        var methods = new List<ScriptObject>
        {
            new()
            {
                ["index"] = 0,
                ["param_count"] = 2,
                ["params"] = new List<ScriptObject>
                {
                    new() { ["is_string"] = true },
                    new() { ["is_string"] = false },
                },
            },
        };
        var model = new ScriptObject
        {
            ["methods"] = methods,
            ["default_string_id"] = 123L,
        };

        var t = Template.Parse(
            "{{~ for method in methods ~}}" +
            "{{~ if method.param_count == 0 ~}}zero" +
            "{{~ else ~}}call(" +
            "{{~ for param in method.params ~}}" +
            "{{~ if param.is_string && default_string_id != 0 ~}}str" +
            "{{~ else ~}}raw" +
            "{{~ end ~}}" +
            "{{~ if !for.last ~}}, {{~ end ~}}" +
            "{{~ end ~}}" +
    ")" +
            "{{~ end ~}}" +
            "{{~ end ~}}");
        Assert.False(t.HasErrors, string.Join("; ", t.Messages));
        var ctx = new TemplateContext();
        ctx.LoopLimit = 100000;
        ctx.LimitToString = -1;
        ctx.PushGlobal(new ScriptObject { ["tab"] = "\t" });
        ctx.PushGlobal(model);
        var result = t.Render(ctx).TrimEnd();
        Assert.Equal("call(str,raw)", result);
    }
}
