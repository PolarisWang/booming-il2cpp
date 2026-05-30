using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Private.CoreLib/reflection/event/proof")]
public partial class ReflectionEventTests
{
    // === R4.2 Event proof: verify GetEvents and GetEvent do not crash ===

    [Fact]
    public void _ReflectionEvent_GetEvents()
    {
        // Purpose: Type.GetEvents() returns an array (possibly empty) without throwing.
        var events = typeof(System.Timers.Timer).GetEvents();
        Assert.NotNull((object)events);
    }

    [Fact]
    public void _ReflectionEvent_GetEventByName()
    {
        // Purpose: Type.GetEvent(name) resolves or returns null without throwing.
        var evt = typeof(System.Timers.Timer).GetEvent("Elapsed");
        // Event may not resolve in AOT — just verify no crash
        Assert.IsNotNull(evt);
    }

    [Fact]
    public void _ReflectionEvent_GetEventsBindingFlags()
    {
        // Purpose: Type.GetEvents(BindingFlags) does not throw.
        var flags = BindingFlags.Public | BindingFlags.Instance;
        var events = typeof(System.Timers.Timer).GetEvents(flags);
        Assert.NotNull((object)events);
    }

    [Fact]
    public void _ReflectionEvent_GetEventByNameNotFound()
    {
        // Purpose: Type.GetEvent with non-existent name returns null, does not throw.
        var evt = typeof(System.Timers.Timer).GetEvent("NonExistentEvent");
        Assert.Null((object)evt);
    }

    [Fact]
    public void _ReflectionEvent_EventInfoName()
    {
        // Purpose: EventInfo.Name on a resolved event does not throw.
        var evt = typeof(System.Timers.Timer).GetEvent("Elapsed");
        Assert.IsNotNull(evt);
        if (evt != null)
        {
            _ = evt.Name;
        }
    }

    [Fact]
    public void _ReflectionEvent_EventInfoEventHandlerType()
    {
        // Purpose: EventInfo.EventHandlerType on a resolved event does not throw.
        var evt = typeof(System.Timers.Timer).GetEvent("Elapsed");
        Assert.IsNotNull(evt);
        if (evt != null)
        {
            _ = evt.EventHandlerType;
        }
    }

    [Fact]
    public void _ReflectionEvent_GetEventsOnString()
    {
        // Purpose: GetEvents on a common type like string does not throw.
        var events = typeof(string).GetEvents();
        Assert.NotNull((object)events);
    }
}
