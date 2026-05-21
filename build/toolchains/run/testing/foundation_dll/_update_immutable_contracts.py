"""Update all 10 System.Collections.Immutable family contracts with real API method subject IDs."""

from __future__ import annotations

import json
from pathlib import Path

IMMUTABLE_DIR = Path("D:/agent/booming-il2cpp/testing/foundation-dll/System.Collections.Immutable")

FAMILY_CONTRACTS: dict[str, list[str]] = {
    "immutable-array": [
        # ImmutableArray.Create<T> static factory methods
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)",
    ],
    "immutable-list": [
        # ImmutableList.Create<T> static factory methods
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Byte>:System.Collections.Immutable.ImmutableList<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Int32>:System.Collections.Immutable.ImmutableList<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<Int32>:System.Collections.Immutable.ImmutableList<System.Int32>(System.Int32)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Create<String>:System.Collections.Immutable.ImmutableList<System.String>(System.String)",
    ],
    "immutable-dictionary": [
        # ImmutableDictionary.Create<TKey,TValue> static factory — empty only (0-param avoids IEqualityComparer overload ambiguity)
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,int>:System.Collections.Immutable.ImmutableDictionary<System.String,System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableDictionary<System.Byte,System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,string>:System.Collections.Immutable.ImmutableDictionary<System.String,System.String>()",
    ],
    "immutable-sorted-dictionary": [
        # ImmutableSortedDictionary.Create<TKey,TValue> — empty only
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()",
    ],
    "immutable-hash-set": [
        # ImmutableHashSet.Create<T> static factory methods
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)",
    ],
    "immutable-sorted-set": [
        # ImmutableSortedSet.Create<T> static factory methods
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Byte>:System.Collections.Immutable.ImmutableSortedSet<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Create<Int32>:System.Collections.Immutable.ImmutableSortedSet<System.Int32>(System.Int32)",
    ],
    "immutable-queue-stack": [
        # ImmutableQueue.Create<T>
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)",
        # ImmutableStack.Create<T>
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)",
        # Enqueue/Dequeue on queue, Push/Pop on stack — instance methods on the struct
    ],
    "immutable-interlocked": [
        # ImmutableInterlocked static methods — T must be a reference type (where T : class)
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})",
    ],
    "frozen-collections": [
        # FrozenDictionary.Create<TKey,TValue> — empty only (avoids IEqualityComparer overload)
        "System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()",
        "System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()",
        "System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()",
        # FrozenSet.Create<T>
        "System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)",
        "System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)",
    ],
    "extensions-marshal": [
        # ImmutableArray.Create<T> — static factory, no extension-method ambiguity
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()",
        "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)",
    ],
}


def update_contract(family_slug: str, method_ids: list[str]) -> None:
    path = IMMUTABLE_DIR / family_slug / "capability-family-contract.json"
    if not path.exists():
        print(f"  [SKIP] {family_slug} — no contract found")
        return

    with open(path, encoding="utf-8") as f:
        contract = json.load(f)

    contract["methodSubjectIds"] = method_ids
    contract["methodSubjectCount"] = len(method_ids)
    # Keep other fields intact

    with open(path, "w", encoding="utf-8") as f:
        json.dump(contract, f, indent=2, ensure_ascii=False)

    print(f"  [OK] {family_slug}: {len(method_ids)} method subject IDs")


def main() -> None:
    print(f"Updating Immutable family contracts in {IMMUTABLE_DIR}")
    for slug, mids in FAMILY_CONTRACTS.items():
        update_contract(slug, mids)

    print(f"\nDone. {len(FAMILY_CONTRACTS)} families updated.")


if __name__ == "__main__":
    main()
