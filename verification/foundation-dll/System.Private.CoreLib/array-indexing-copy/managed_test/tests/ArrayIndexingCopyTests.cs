using System;
using Xunit;

namespace ArrayIndexingCopyTests;

public class ArrayIndexingCopyTests
{
    // === Array.Copy(System.Array, System.Array, System.Int32) ===

    [Fact]
    public void Copy_ArrayArrayInt32_CopiesElements()
    {
        byte[] source = { 1, 2, 3, 4, 5 };
        byte[] dest = new byte[5];
        Array.Copy(source, dest, 5);
        Assert.Equal(1, dest[0]);
        Assert.Equal(5, dest[4]);
    }

    [Fact]
    public void Copy_ArrayArrayInt32_PartialCopy()
    {
        byte[] source = { 1, 2, 3, 4, 5 };
        byte[] dest = new byte[3];
        Array.Copy(source, dest, 3);
        Assert.Equal(1, dest[0]);
        Assert.Equal(3, dest[2]);
    }

    // === Array.Copy(System.Array, System.Int32, System.Array, System.Int32, System.Int32) ===

    [Fact]
    public void Copy_ArrayInt32ArrayInt32Int32_CopiesWithOffsets()
    {
        byte[] source = { 1, 2, 3, 4, 5 };
        byte[] dest = new byte[3];
        Array.Copy(source, 2, dest, 0, 3);
        Assert.Equal(3, dest[0]);
        Assert.Equal(5, dest[2]);
    }

    [Fact]
    public void Copy_ArrayInt32ArrayInt32Int32_CopiesToMiddle()
    {
        byte[] source = { 9, 9, 9 };
        byte[] dest = { 1, 2, 3, 4, 5 };
        Array.Copy(source, 0, dest, 1, 3);
        Assert.Equal(1, dest[0]);
        Assert.Equal(9, dest[1]);
        Assert.Equal(9, dest[3]);
        Assert.Equal(5, dest[4]);
    }

    // === Array.Clear(System.Array, System.Int32, System.Int32) ===

    [Fact]
    public void Clear_ArrayInt32Int32_ClearsElements()
    {
        byte[] arr = { 1, 2, 3, 4, 5 };
        Array.Clear(arr, 1, 3);
        Assert.Equal(1, arr[0]);
        Assert.Equal(0, arr[1]);
        Assert.Equal(0, arr[2]);
        Assert.Equal(0, arr[3]);
        Assert.Equal(5, arr[4]);
    }

    [Fact]
    public void Clear_ArrayInt32Int32_WholeArray()
    {
        byte[] arr = { 1, 2, 3 };
        Array.Clear(arr, 0, 3);
        Assert.Equal(0, arr[0]);
        Assert.Equal(0, arr[2]);
    }

    // === Array.Resize (ref parameter — needs-manual) ===

    [Fact(Skip = "needs-manual — ref parameter requires unsafe context")]
    public void Resize_RefTArrayInt32_NeedsManual()
    {
        // TODO: Implement test for Array.Resize with ref parameter
    }

    // === Array.Sort(System.Array) ===

    [Fact]
    public void Sort_Array_SortsInPlace()
    {
        byte[] arr = { 3, 1, 4, 1, 5, 9, 2, 6 };
        Array.Sort(arr);
        Assert.Equal(1, arr[0]);
        Assert.Equal(1, arr[1]);
        Assert.Equal(2, arr[2]);
        Assert.Equal(9, arr[7]);
    }

    [Fact]
    public void Sort_Array_SortsSingleElement()
    {
        byte[] arr = { 42 };
        Array.Sort(arr);
        Assert.Equal((byte)42, arr[0]);
    }

    // === Array.Sort(System.Array, System.Collections.IComparer) ===

    [Fact]
    public void Sort_ArrayIComparer_SortsWithNullComparer()
    {
        byte[] arr = { 3, 1, 2 };
        Array.Sort(arr, null!);
        Assert.Equal(1, arr[0]);
        Assert.Equal(2, arr[1]);
        Assert.Equal(3, arr[2]);
    }

    // === Array.BinarySearch(System.Array, System.Object) ===

    [Fact]
    public void BinarySearch_ArrayObject_ReturnsIndex()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        int index = Array.BinarySearch(arr, (object)(byte)30);
        Assert.Equal(2, index);
    }

    [Fact]
    public void BinarySearch_ArrayObject_NotFoundReturnsNegative()
    {
        byte[] arr = { 10, 20, 30 };
        int index = Array.BinarySearch(arr, (object)(byte)99);
        Assert.True(index < 0);
    }

    // === Array.BinarySearch(System.Array, System.Int32, System.Int32, System.Object) ===

    [Fact]
    public void BinarySearch_ArrayInt32Int32Object_ReturnsIndexInRange()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        int index = Array.BinarySearch(arr, 0, 3, (object)(byte)20);
        Assert.Equal(1, index);
    }

    [Fact]
    public void BinarySearch_ArrayInt32Int32Object_SearchSubset()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        int index = Array.BinarySearch(arr, 3, 2, (object)(byte)50);
        Assert.Equal(4, index);
    }

    // === Array.IndexOf(System.Array, System.Object) ===

    [Fact]
    public void IndexOf_ArrayObject_ReturnsFirstIndex()
    {
        byte[] arr = { 5, 3, 5, 3, 5 };
        int index = Array.IndexOf(arr, (object)(byte)3);
        Assert.Equal(1, index);
    }

    [Fact]
    public void IndexOf_ArrayObject_NotFoundReturnsMinusOne()
    {
        byte[] arr = { 1, 2, 3 };
        int index = Array.IndexOf(arr, (object)(byte)99);
        Assert.Equal(-1, index);
    }

    [Fact]
    public void IndexOf_ArrayObject_FirstElementMatch()
    {
        byte[] arr = { 7, 8, 9 };
        int index = Array.IndexOf(arr, (object)(byte)7);
        Assert.Equal(0, index);
    }

    // === Array.LastIndexOf(System.Array, System.Object) ===

    [Fact]
    public void LastIndexOf_ArrayObject_ReturnsLastIndex()
    {
        byte[] arr = { 5, 3, 5, 3, 5 };
        int index = Array.LastIndexOf(arr, (object)(byte)3);
        Assert.Equal(3, index);
    }

    [Fact]
    public void LastIndexOf_ArrayObject_SingleMatch()
    {
        byte[] arr = { 1, 2, 3, 4, 5 };
        int index = Array.LastIndexOf(arr, (object)(byte)1);
        Assert.Equal(0, index);
    }

    // === Array.Reverse(System.Array) ===

    [Fact]
    public void Reverse_Array_ReversesInPlace()
    {
        byte[] arr = { 1, 2, 3, 4, 5 };
        Array.Reverse(arr);
        Assert.Equal(5, arr[0]);
        Assert.Equal(4, arr[1]);
        Assert.Equal(3, arr[2]);
        Assert.Equal(2, arr[3]);
        Assert.Equal(1, arr[4]);
    }

    [Fact]
    public void Reverse_Array_EmptyArray()
    {
        byte[] arr = Array.Empty<byte>();
        Array.Reverse(arr);
        Assert.Empty(arr);
    }

    // === Array.GetLength(System.Int32) ===

    [Fact]
    public void GetLength_Int32_ReturnsDimensionLength()
    {
        Array arr = Array.CreateInstance(typeof(byte), 3, 4);
        int len0 = arr.GetLength(0);
        int len1 = arr.GetLength(1);
        Assert.Equal(3, len0);
        Assert.Equal(4, len1);
    }

    [Fact]
    public void GetLength_Int32_SingleDimension()
    {
        byte[] arr = new byte[7];
        int len = arr.GetLength(0);
        Assert.Equal(7, len);
    }

    // === Array.GetValue(System.Int32) ===

    [Fact]
    public void GetValue_Int32_ReturnsElementValue()
    {
        byte[] arr = { 10, 20, 30 };
        object val = arr.GetValue(1);
        Assert.Equal((byte)20, val);
    }

    [Fact]
    public void GetValue_Int32_FirstElement()
    {
        byte[] arr = { 42, 99 };
        object val = arr.GetValue(0);
        Assert.Equal((byte)42, val);
    }
}