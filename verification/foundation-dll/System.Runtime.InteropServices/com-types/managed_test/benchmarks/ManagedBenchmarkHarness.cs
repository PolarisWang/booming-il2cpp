// Auto-generated managed benchmark harness
// Family: com-types, Assembly: System.Runtime.InteropServices
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static volatile int _g;  // volatile side-effect prevents JIT DCE

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }



    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(System.Runtime.InteropServices.ComTypes.IMoniker) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(System.Runtime.InteropServices.ComTypes.IMoniker)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Runtime.InteropServices.ComTypes.IEnumFORMATETC(System.Runtime.InteropServices.ComTypes.DATADIR) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Runtime.InteropServices.ComTypes.IEnumFORMATETC(System.Runtime.InteropServices.ComTypes.DATADIR)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.FORMATETC{},System.Int32{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.FORMATETC{},System.Int32{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Int32(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Int32(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.STATDATA{},System.Int32{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.STATDATA{},System.Int32{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Int32(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Int32(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
