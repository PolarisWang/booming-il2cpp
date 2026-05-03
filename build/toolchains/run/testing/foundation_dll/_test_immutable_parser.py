import sys
sys.path.insert(0, 'D:/agent/booming-il2cpp/build/toolchains/run/testing/foundation_dll')
from test_code_generator import _parse_method_subject_id, _build_call_expr, _is_auto_callable, _has_blocked_param

tests = [
    ("Interlocked.Update via Func", "System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<Byte>:System.Boolean(System.Byte&,System.Func{System.Byte,System.Byte})"),
    ("Interlocked.Exchange via ref", "System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::InterlockedExchange<Byte>:System.Byte(System.Byte&,System.Byte)"),
    ("ImmutableArrayExtensions.ToImmutableArray", "System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::ToImmutableArray<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Collections.Generic.IEnumerable{System.Byte})"),
    ("ImmutableArray indexer", "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray`1::get_Item:System.Byte(System.Int32)"),
    ("FrozenDictionary.ToFrozenDict", "System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>(System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{System.String,System.Int32}})"),
]
for label, sid in tests:
    parsed = _parse_method_subject_id(sid)
    auto = _is_auto_callable(parsed)
    blocked = _has_blocked_param(parsed['param_types'])
    print(f"{label}: auto={auto} blocked={blocked}")
    if auto:
        print(f"  expr: {_build_call_expr(parsed)}")
    print()
