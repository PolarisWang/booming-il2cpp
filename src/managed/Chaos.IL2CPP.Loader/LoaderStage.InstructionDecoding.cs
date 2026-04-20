using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static ManagedMethodBodyModel DecodeMethodBody(
        PEReader peReader,
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        MethodDefinitionHandle methodHandle,
        MethodSummary methodSummary)
    {
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);
        if (methodDefinition.RelativeVirtualAddress == 0)
        {
            return new ManagedMethodBodyModel
            {
                ExceptionRegions = [],
                Blocks =
                [
                    new ManagedBlockModel
                    {
                        BlockId = "entry",
                        Instructions = [],
                    },
                ],
            };
        }

        var bodyBlock = peReader.GetMethodBody(methodDefinition.RelativeVirtualAddress);
        var ilReader = bodyBlock.GetILReader();
        var instructions = new List<ManagedInstructionModel>();

        while (ilReader.RemainingBytes > 0)
        {
            var instructionOffset = ilReader.Offset;
            var opCode = ReadOpCode(ref ilReader);
            var instruction = DecodeInstruction(
                metadataReader,
                typeResolver,
                typeModels,
                fieldOwners,
                methodOwners,
                methodSummary,
                opCode,
                ref ilReader);

            if (instruction is not null)
            {
                instructions.Add(instruction with { IlOffset = instructionOffset });
            }
        }

        return new ManagedMethodBodyModel
        {
            // ExceptionRegions = bodyBlock.ExceptionRegions are decoded into the contracts model.
            ExceptionRegions = DecodeExceptionRegions(bodyBlock, typeResolver),
            Blocks =
            [
                new ManagedBlockModel
                {
                    BlockId = "entry",
                    Instructions = instructions,
                },
            ],
        };
    }

    private static ManagedInstructionModel? DecodeInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        MethodSummary methodSummary,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        return opCode switch
        {
            ILOpCode.Nop => null,
            ILOpCode.Volatile => null,
            ILOpCode.Readonly => null,
            ILOpCode.Constrained => SkipConstrainedInstruction(ref ilReader),
            ILOpCode.Unaligned => SkipUnalignedInstruction(ref ilReader),
            ILOpCode.Ldtoken => DecodeLdtokenInstruction(metadataReader, typeResolver, typeModels, fieldOwners, methodOwners, ref ilReader),
            ILOpCode.Ldstr => DecodeLdstrInstruction(metadataReader, ref ilReader),
            ILOpCode.Ldftn => DecodeLdftnInstruction(metadataReader, typeResolver, typeModels, methodOwners, ref ilReader),
            ILOpCode.Ldvirtftn => DecodeLdvirtftnInstruction(metadataReader, typeResolver, typeModels, methodOwners, ref ilReader),
            ILOpCode.Ldnull => new ManagedInstructionModel { Op = "ldnull", ResultType = "System.Object" },
            ILOpCode.Newarr => DecodeNewarrInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Ldlen => new ManagedInstructionModel { Op = "ldlen", ResultType = "System.IntPtr" },
            ILOpCode.Castclass => DecodeCastclassInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Isinst => DecodeIsinstInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Initobj => DecodeInitobjInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Ldobj => DecodeLdobjInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Stobj => DecodeStobjInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Mkrefany => DecodeMkrefanyInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Refanytype => DecodeSimpleInstruction("refanytype", "System.RuntimeTypeHandle"),
            ILOpCode.Refanyval => DecodeRefanyvalInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Sizeof => DecodeSizeofInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Arglist => new ManagedInstructionModel { Op = "arglist", ResultType = "System.RuntimeArgumentHandle" },
            ILOpCode.Newobj => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Box => DecodeBoxInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Unbox => DecodeUnboxInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Unbox_any => DecodeUnboxAnyInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Calli => DecodeCalliInstruction(metadataReader, typeResolver, methodSummary, ref ilReader),
            ILOpCode.Call => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Callvirt => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Stfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldflda => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Stsfld => DecodeStsfldInstruction(metadataReader, typeResolver, fieldOwners, ref ilReader),
            ILOpCode.Ldsfld => DecodeLdsfldInstruction(metadataReader, typeResolver, fieldOwners, ref ilReader),
            ILOpCode.Ldsflda => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldelema => DecodeLdelemaInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Stelem_ref => new ManagedInstructionModel { Op = "stelem.ref", ResultType = "System.Void" },
            ILOpCode.Stelem => DecodeArrayElementInstruction(metadataReader, typeResolver, ILOpCode.Stelem, ref ilReader),
            ILOpCode.Stelem_i1 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.SByte"),
            ILOpCode.Stelem_i2 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Int16"),
            ILOpCode.Stelem_i4 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Int32"),
            ILOpCode.Stelem_i => DecodeTypedArrayInstruction("stelem", "System.Void", "System.IntPtr"),
            ILOpCode.Stelem_i8 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Int64"),
            ILOpCode.Stelem_r4 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Single"),
            ILOpCode.Ldelem_i4 => DecodeTypedArrayInstruction("ldelem", "System.Int32", "System.Int32"),
            ILOpCode.Ldelem_i1 => DecodeTypedArrayInstruction("ldelem", "System.SByte", "System.SByte"),
            ILOpCode.Ldelem_u1 => DecodeTypedArrayInstruction("ldelem", "System.Byte", "System.Byte"),
            ILOpCode.Ldelem_i2 => DecodeTypedArrayInstruction("ldelem", "System.Int16", "System.Int16"),
            ILOpCode.Ldelem_u2 => DecodeTypedArrayInstruction("ldelem", "System.UInt16", "System.UInt16"),
            ILOpCode.Ldelem_u4 => DecodeTypedArrayInstruction("ldelem", "System.UInt32", "System.UInt32"),
            ILOpCode.Ldelem_i8 => DecodeTypedArrayInstruction("ldelem", "System.Int64", "System.Int64"),
            ILOpCode.Ldelem_i => DecodeTypedArrayInstruction("ldelem", "System.IntPtr", "System.IntPtr"),
            ILOpCode.Ldelem_r4 => DecodeTypedArrayInstruction("ldelem", "System.Single", "System.Single"),
            ILOpCode.Stelem_r8 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Double"),
            ILOpCode.Ldelem_r8 => DecodeTypedArrayInstruction("ldelem", "System.Double", "System.Double"),
            ILOpCode.Dup => new ManagedInstructionModel { Op = "dup" },
            ILOpCode.Pop => new ManagedInstructionModel { Op = "pop" },
            ILOpCode.Ldelem => DecodeArrayElementInstruction(metadataReader, typeResolver, ILOpCode.Ldelem, ref ilReader),
            ILOpCode.Ldelem_ref => new ManagedInstructionModel { Op = "ldelem.ref", ResultType = "System.Object" },
            ILOpCode.Ldarg => DecodeLdargInstruction(methodSummary, ilReader.ReadUInt16()),
            ILOpCode.Ldarg_s => DecodeLdargInstruction(methodSummary, ilReader.ReadByte()),
            ILOpCode.Ldarg_0 => DecodeLdargInstruction(methodSummary, 0),
            ILOpCode.Ldarg_1 => DecodeLdargInstruction(methodSummary, 1),
            ILOpCode.Ldarg_2 => DecodeLdargInstruction(methodSummary, 2),
            ILOpCode.Ldarg_3 => DecodeLdargInstruction(methodSummary, 3),
            ILOpCode.Starg => DecodeStargInstruction(ilReader.ReadUInt16()),
            ILOpCode.Starg_s => DecodeStargInstruction(ilReader.ReadByte()),
            ILOpCode.Ldarga => DecodeLdargaInstruction(ilReader.ReadUInt16()),
            ILOpCode.Ldarga_s => DecodeLdargaInstruction(ilReader.ReadByte()),
            ILOpCode.Ldloc => DecodeLdlocInstruction(ilReader.ReadUInt16()),
            ILOpCode.Ldloc_s => DecodeLdlocInstruction(ilReader.ReadByte()),
            ILOpCode.Ldloc_0 => DecodeLdlocInstruction(0),
            ILOpCode.Ldloc_1 => DecodeLdlocInstruction(1),
            ILOpCode.Ldloc_2 => DecodeLdlocInstruction(2),
            ILOpCode.Ldloc_3 => DecodeLdlocInstruction(3),
            ILOpCode.Ldloca => DecodeLdlocaInstruction(ilReader.ReadUInt16()),
            ILOpCode.Ldloca_s => DecodeLdlocaInstruction(ilReader.ReadByte()),
            ILOpCode.Stloc => DecodeStlocInstruction(ilReader.ReadUInt16()),
            ILOpCode.Stloc_s => DecodeStlocInstruction(ilReader.ReadByte()),
            ILOpCode.Stloc_0 => DecodeStlocInstruction(0),
            ILOpCode.Stloc_1 => DecodeStlocInstruction(1),
            ILOpCode.Stloc_2 => DecodeStlocInstruction(2),
            ILOpCode.Stloc_3 => DecodeStlocInstruction(3),
            ILOpCode.Ldc_i4 => DecodeLdcI4Instruction(ilReader.ReadInt32()),
            ILOpCode.Ldc_i4_s => DecodeLdcI4Instruction(ilReader.ReadSByte()),
            ILOpCode.Ldc_i4_m1 => DecodeLdcI4Instruction(-1),
            ILOpCode.Ldc_i4_0 => DecodeLdcI4Instruction(0),
            ILOpCode.Ldc_i4_1 => DecodeLdcI4Instruction(1),
            ILOpCode.Ldc_i4_2 => DecodeLdcI4Instruction(2),
            ILOpCode.Ldc_i4_3 => DecodeLdcI4Instruction(3),
            ILOpCode.Ldc_i4_4 => DecodeLdcI4Instruction(4),
            ILOpCode.Ldc_i4_5 => DecodeLdcI4Instruction(5),
            ILOpCode.Ldc_i4_6 => DecodeLdcI4Instruction(6),
            ILOpCode.Ldc_i4_7 => DecodeLdcI4Instruction(7),
            ILOpCode.Ldc_i4_8 => DecodeLdcI4Instruction(8),
            ILOpCode.Ldc_i8 => DecodeLdcI8Instruction(ilReader.ReadInt64()),
            ILOpCode.Ldc_r4 => DecodeLdcR4Instruction(ilReader.ReadSingle()),
            ILOpCode.Ldc_r8 => DecodeLdcR8Instruction(ilReader.ReadDouble()),
            ILOpCode.Ldind_i1 => DecodeSimpleInstruction("ldind.i1", "System.SByte"),
            ILOpCode.Ldind_u1 => DecodeSimpleInstruction("ldind.u1", "System.Byte"),
            ILOpCode.Ldind_i2 => DecodeSimpleInstruction("ldind.i2", "System.Int16"),
            ILOpCode.Ldind_u2 => DecodeSimpleInstruction("ldind.u2", "System.UInt16"),
            ILOpCode.Ldind_i4 => DecodeSimpleInstruction("ldind.i4", "System.Int32"),
            ILOpCode.Ldind_u4 => DecodeSimpleInstruction("ldind.u4", "System.UInt32"),
            ILOpCode.Ldind_i8 => DecodeSimpleInstruction("ldind.i8", "System.Int64"),
            ILOpCode.Ldind_i => DecodeSimpleInstruction("ldind.i", "System.IntPtr"),
            ILOpCode.Ldind_r4 => DecodeSimpleInstruction("ldind.r4", "System.Single"),
            ILOpCode.Ldind_r8 => DecodeSimpleInstruction("ldind.r8", "System.Double"),
            ILOpCode.Ldind_ref => DecodeSimpleInstruction("ldind.ref", "System.Object"),
            ILOpCode.Stind_i1 => DecodeSimpleInstruction("stind.i1", "System.Void"),
            ILOpCode.Stind_i2 => DecodeSimpleInstruction("stind.i2", "System.Void"),
            ILOpCode.Stind_i4 => DecodeSimpleInstruction("stind.i4", "System.Void"),
            ILOpCode.Stind_i8 => DecodeSimpleInstruction("stind.i8", "System.Void"),
            ILOpCode.Stind_i => DecodeSimpleInstruction("stind.i", "System.Void"),
            ILOpCode.Stind_r4 => DecodeSimpleInstruction("stind.r4", "System.Void"),
            ILOpCode.Stind_r8 => DecodeSimpleInstruction("stind.r8", "System.Void"),
            ILOpCode.Stind_ref => DecodeSimpleInstruction("stind.ref", "System.Void"),
            ILOpCode.Cpblk => new ManagedInstructionModel { Op = "cpblk", ResultType = "System.Void" },
            ILOpCode.Localloc => new ManagedInstructionModel { Op = "localloc", ResultType = "System.IntPtr" },
            ILOpCode.Conv_i => DecodeSimpleInstruction("conv.i", "System.IntPtr"),
            ILOpCode.Conv_i1 => DecodeSimpleInstruction("conv.i1", "System.SByte"),
            ILOpCode.Conv_i2 => DecodeSimpleInstruction("conv.i2", "System.Int16"),
            ILOpCode.Conv_i4 => new ManagedInstructionModel { Op = "conv.i4", ResultType = "System.Int32" },
            ILOpCode.Conv_i8 => DecodeSimpleInstruction("conv.i8", "System.Int64"),
            ILOpCode.Conv_u4 => DecodeSimpleInstruction("conv.u4", "System.UInt32"),
            ILOpCode.Conv_ovf_i1 => DecodeSimpleInstruction("conv.ovf.i1", "System.SByte"),
            ILOpCode.Conv_ovf_i2 => DecodeSimpleInstruction("conv.ovf.i2", "System.Int16"),
            ILOpCode.Conv_ovf_i4 => DecodeSimpleInstruction("conv.ovf.i4", "System.Int32"),
            ILOpCode.Conv_ovf_i8 => DecodeSimpleInstruction("conv.ovf.i8", "System.Int64"),
            ILOpCode.Conv_ovf_u1 => DecodeSimpleInstruction("conv.ovf.u1", "System.Byte"),
            ILOpCode.Conv_ovf_u2 => DecodeSimpleInstruction("conv.ovf.u2", "System.UInt16"),
            ILOpCode.Conv_ovf_u4 => DecodeSimpleInstruction("conv.ovf.u4", "System.UInt32"),
            ILOpCode.Conv_ovf_u8 => DecodeSimpleInstruction("conv.ovf.u8", "System.UInt64"),
            ILOpCode.Conv_ovf_i1_un => DecodeSimpleInstruction("conv.ovf.i1.un", "System.SByte"),
            ILOpCode.Conv_ovf_i2_un => DecodeSimpleInstruction("conv.ovf.i2.un", "System.Int16"),
            ILOpCode.Conv_ovf_i4_un => DecodeSimpleInstruction("conv.ovf.i4.un", "System.Int32"),
            ILOpCode.Conv_ovf_i8_un => DecodeSimpleInstruction("conv.ovf.i8.un", "System.Int64"),
            ILOpCode.Conv_ovf_u1_un => DecodeSimpleInstruction("conv.ovf.u1.un", "System.Byte"),
            ILOpCode.Conv_ovf_u2_un => DecodeSimpleInstruction("conv.ovf.u2.un", "System.UInt16"),
            ILOpCode.Conv_ovf_u4_un => DecodeSimpleInstruction("conv.ovf.u4.un", "System.UInt32"),
            ILOpCode.Conv_ovf_u8_un => DecodeSimpleInstruction("conv.ovf.u8.un", "System.UInt64"),
            ILOpCode.Conv_ovf_i => DecodeSimpleInstruction("conv.ovf.i", "System.IntPtr"),
            ILOpCode.Conv_ovf_i_un => DecodeSimpleInstruction("conv.ovf.i.un", "System.IntPtr"),
            ILOpCode.Conv_ovf_u => DecodeSimpleInstruction("conv.ovf.u", "System.UIntPtr"),
            ILOpCode.Conv_ovf_u_un => DecodeSimpleInstruction("conv.ovf.u.un", "System.UIntPtr"),
            ILOpCode.Conv_r_un => DecodeSimpleInstruction("conv.r.un", "System.Double"),
            ILOpCode.Conv_r4 => DecodeSimpleInstruction("conv.r4", "System.Single"),
            ILOpCode.Conv_r8 => DecodeSimpleInstruction("conv.r8", "System.Double"),
            ILOpCode.Conv_u => DecodeSimpleInstruction("conv.u", "System.IntPtr"),
            ILOpCode.Conv_u1 => DecodeSimpleInstruction("conv.u1", "System.Byte"),
            ILOpCode.Conv_u2 => DecodeSimpleInstruction("conv.u2", "System.UInt16"),
            ILOpCode.Conv_u8 => DecodeSimpleInstruction("conv.u8", "System.UInt64"),
            ILOpCode.Add => DecodeSimpleInstruction("add", "System.Int32"),
            ILOpCode.Add_ovf => DecodeSimpleInstruction("add.ovf", "System.Int32"),
            ILOpCode.Add_ovf_un => DecodeSimpleInstruction("add.ovf.un", "System.Int32"),
            ILOpCode.And => DecodeSimpleInstruction("and", "System.Int32"),
            ILOpCode.Neg => DecodeSimpleInstruction("neg", "System.Int32"),
            ILOpCode.Sub => DecodeSimpleInstruction("sub", "System.Int32"),
            ILOpCode.Sub_ovf => DecodeSimpleInstruction("sub.ovf", "System.Int32"),
            ILOpCode.Sub_ovf_un => DecodeSimpleInstruction("sub.ovf.un", "System.Int32"),
            ILOpCode.Mul => DecodeSimpleInstruction("mul", "System.Int32"),
            ILOpCode.Mul_ovf => DecodeSimpleInstruction("mul.ovf", "System.Int32"),
            ILOpCode.Mul_ovf_un => DecodeSimpleInstruction("mul.ovf.un", "System.Int32"),
            ILOpCode.Div => DecodeSimpleInstruction("div", "System.Int32"),
            ILOpCode.Div_un => DecodeSimpleInstruction("div.un", "System.Int32"),
            ILOpCode.Rem => DecodeSimpleInstruction("rem", "System.Int32"),
            ILOpCode.Rem_un => DecodeSimpleInstruction("rem.un", "System.Int32"),
            ILOpCode.Ceq => DecodeSimpleInstruction("ceq", "System.Int32"),
            ILOpCode.Clt => DecodeSimpleInstruction("clt", "System.Int32"),
            ILOpCode.Clt_un => DecodeSimpleInstruction("clt.un", "System.Int32"),
            ILOpCode.Cgt => DecodeSimpleInstruction("cgt", "System.Int32"),
            ILOpCode.Cgt_un => DecodeSimpleInstruction("cgt.un", "System.Int32"),
            ILOpCode.Not => DecodeSimpleInstruction("not", "System.Int32"),
            ILOpCode.Or => DecodeSimpleInstruction("or", "System.Int32"),
            ILOpCode.Shl => DecodeSimpleInstruction("shl", "System.Int32"),
            ILOpCode.Shr => DecodeSimpleInstruction("shr", "System.Int32"),
            ILOpCode.Shr_un => DecodeSimpleInstruction("shr.un", "System.Int32"),
            ILOpCode.Xor => DecodeSimpleInstruction("xor", "System.Int32"),
            ILOpCode.Beq => DecodeBranchInstruction("beq", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Beq_s => DecodeBranchInstruction("beq", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Br => DecodeBranchInstruction("br", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Br_s => DecodeBranchInstruction("br", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bne_un => DecodeBranchInstruction("bne.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bne_un_s => DecodeBranchInstruction("bne.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Brtrue => DecodeBrtrueInstruction(ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Brtrue_s => DecodeBrtrueInstruction(ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Brfalse => DecodeBrfalseInstruction(ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Brfalse_s => DecodeBrfalseInstruction(ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Blt => DecodeBranchInstruction("blt", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Blt_s => DecodeBranchInstruction("blt", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Blt_un => DecodeBranchInstruction("blt.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Blt_un_s => DecodeBranchInstruction("blt.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bgt => DecodeBranchInstruction("bgt", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bgt_s => DecodeBranchInstruction("bgt", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bgt_un => DecodeBranchInstruction("bgt.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bgt_un_s => DecodeBranchInstruction("bgt.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Ble => DecodeBranchInstruction("ble", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Ble_s => DecodeBranchInstruction("ble", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Ble_un => DecodeBranchInstruction("ble.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Ble_un_s => DecodeBranchInstruction("ble.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bge => DecodeBranchInstruction("bge", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bge_s => DecodeBranchInstruction("bge", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bge_un => DecodeBranchInstruction("bge.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bge_un_s => DecodeBranchInstruction("bge.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Switch => DecodeSwitchInstruction(ref ilReader),
            ILOpCode.Leave => DecodeLeaveInstruction(ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Leave_s => DecodeLeaveInstruction(ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Throw => new ManagedInstructionModel { Op = "throw", ResultType = "System.Void" },
            ILOpCode.Rethrow => new ManagedInstructionModel { Op = "rethrow", ResultType = "System.Void" },
            ILOpCode.Endfilter => new ManagedInstructionModel { Op = "endfilter", ResultType = "System.Void" },
            ILOpCode.Endfinally => new ManagedInstructionModel { Op = "endfinally", ResultType = "System.Void" },
            ILOpCode.Ret => new ManagedInstructionModel { Op = "ret" },
            _ => throw new NotSupportedException($"unsupported IL opcode in loader: {opCode}"),
        };
    }

    private static ManagedInstructionModel DecodeTypeReferenceInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var typeIdentity = typeResolver.ResolveTypeIdentity(handle);

        return new ManagedInstructionModel
        {
            Op = opCode switch
            {
                ILOpCode.Newarr => "newarr",
                ILOpCode.Box => "box",
                ILOpCode.Castclass => "castclass",
                ILOpCode.Isinst => "isinst",
                ILOpCode.Initobj => "initobj",
                ILOpCode.Ldobj => "ldobj",
                ILOpCode.Mkrefany => "mkrefany",
                ILOpCode.Refanyval => "refanyval",
                ILOpCode.Sizeof => "sizeof",
                ILOpCode.Ldelema => "ldelema",
                ILOpCode.Ldelem => "ldelem",
                ILOpCode.Stobj => "stobj",
                ILOpCode.Stelem => "stelem",
                ILOpCode.Unbox => "unbox",
                ILOpCode.Unbox_any => "unbox.any",
                _ => throw new NotSupportedException($"unsupported type reference opcode: {opCode}"),
            },
            Operand = typeIdentity.SubjectId,
            ResultType = opCode switch
            {
                ILOpCode.Newarr => $"{typeIdentity.DisplayName}[]",
                ILOpCode.Box => "System.Object",
                ILOpCode.Castclass => typeIdentity.SubjectId,
                ILOpCode.Isinst => typeIdentity.SubjectId,
                ILOpCode.Initobj => "System.Void",
                ILOpCode.Ldobj => typeIdentity.SubjectId,
                ILOpCode.Mkrefany => "System.TypedReference",
                ILOpCode.Refanyval => "System.IntPtr",
                ILOpCode.Sizeof => "System.Int32",
                ILOpCode.Ldelema => "System.IntPtr",
                ILOpCode.Ldelem => typeIdentity.SubjectId,
                ILOpCode.Stobj => "System.Void",
                ILOpCode.Stelem => "System.Void",
                ILOpCode.Unbox => "System.IntPtr",
                ILOpCode.Unbox_any => typeIdentity.SubjectId,
                _ => throw new NotSupportedException($"unsupported type reference opcode: {opCode}"),
            },
            Reference = new ManagedInstructionReference
            {
                AssemblyName = typeIdentity.AssemblyName,
                SubjectKind = "type",
                SubjectId = typeIdentity.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeNewarrInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Newarr, ref ilReader);
        return instruction with { Op = "newarr" };
    }

    private static ManagedInstructionModel DecodeBoxInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Box, ref ilReader);
        return instruction with { Op = "box" };
    }

    private static ManagedInstructionModel DecodeUnboxInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Unbox, ref ilReader);
        return instruction with { Op = "unbox" };
    }

    private static ManagedInstructionModel DecodeUnboxAnyInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Unbox_any, ref ilReader);
        return instruction with { Op = "unbox.any" };
    }

    private static ManagedInstructionModel DecodeLdelemaInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Ldelema, ref ilReader);
        return instruction with { Op = "ldelema" };
    }

    private static ManagedInstructionModel DecodeArrayElementInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, opCode, ref ilReader);
        return instruction with
        {
            Op = opCode == ILOpCode.Ldelem ? "ldelem" : "stelem",
        };
    }

    private static ManagedInstructionModel DecodeCastclassInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Castclass, ref ilReader);
        return instruction with { Op = "castclass" };
    }

    private static ManagedInstructionModel DecodeIsinstInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Isinst, ref ilReader);
        return instruction with { Op = "isinst" };
    }

    private static ManagedInstructionModel DecodeInitobjInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Initobj, ref ilReader);
        return instruction with { Op = "initobj" };
    }

    private static ManagedInstructionModel DecodeLdobjInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Ldobj, ref ilReader);
        return instruction with { Op = "ldobj" };
    }

    private static ManagedInstructionModel DecodeMkrefanyInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Mkrefany, ref ilReader);
        return instruction with { Op = "mkrefany" };
    }

    private static ManagedInstructionModel DecodeRefanyvalInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Refanyval, ref ilReader);
        return instruction with { Op = "refanyval" };
    }

    private static ManagedInstructionModel DecodeSizeofInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Sizeof, ref ilReader);
        return instruction with { Op = "sizeof" };
    }

    private static ManagedInstructionModel DecodeStobjInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Stobj, ref ilReader);
        return instruction with { Op = "stobj" };
    }

    private static ManagedInstructionModel DecodeTypedArrayInstruction(string op, string resultType, string elementType)
    {
        return new ManagedInstructionModel
        {
            Op = op,
            Operand = elementType,
            ResultType = resultType,
        };
    }

    private static ManagedInstructionModel DecodeLdtokenInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);

        return handle.Kind switch
        {
            HandleKind.TypeDefinition or HandleKind.TypeReference or HandleKind.TypeSpecification
                => CreateLdtokenTypeInstruction(typeResolver.ResolveTypeIdentity(handle)),
            HandleKind.FieldDefinition or HandleKind.MemberReference
                => CreateLdtokenFieldInstruction(ResolveFieldReference(metadataReader, typeResolver, fieldOwners, handle)),
            HandleKind.MethodDefinition or HandleKind.MethodSpecification
                => CreateLdtokenMethodInstruction(ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, handle)),
            _ => throw new NotSupportedException($"unsupported ldtoken handle kind in loader: {handle.Kind}"),
        };
    }

    private static ManagedInstructionModel CreateLdtokenTypeInstruction(TypeIdentity typeIdentity)
    {
        return new ManagedInstructionModel
        {
            Op = "ldtoken",
            Operand = typeIdentity.SubjectId,
            ResultType = "System.RuntimeTypeHandle",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = typeIdentity.AssemblyName,
                SubjectKind = "type",
                SubjectId = typeIdentity.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel CreateLdtokenFieldInstruction(FieldReferenceSummary reference)
    {
        return new ManagedInstructionModel
        {
            Op = "ldtoken",
            Operand = reference.SubjectId,
            ResultType = "System.RuntimeFieldHandle",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "field",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel CreateLdtokenMethodInstruction(MethodReferenceSummary reference)
    {
        return new ManagedInstructionModel
        {
            Op = "ldtoken",
            Operand = reference.SubjectId,
            ResultType = "System.RuntimeMethodHandle",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "method",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeLdstrInstruction(MetadataReader metadataReader, ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var value = metadataReader.GetUserString(MetadataTokens.UserStringHandle(token));

        return new ManagedInstructionModel
        {
            Op = "ldstr",
            Operand = value,
            ResultType = "System.String",
        };
    }

    private static ManagedInstructionModel DecodeMethodReferenceInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var reference = ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, handle);

        return new ManagedInstructionModel
        {
            Op = opCode switch
            {
                ILOpCode.Newobj => "newobj",
                ILOpCode.Call => "call",
                ILOpCode.Callvirt => "callvirt",
                _ => throw new NotSupportedException($"unsupported method reference opcode: {opCode}"),
            },
            ResultType = opCode == ILOpCode.Newobj ? reference.DeclaringTypeSubjectId : reference.ReturnType,
            Callee = reference.SubjectId,
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "method",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeFunctionPointerInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string op,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var reference = ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, handle);

        return new ManagedInstructionModel
        {
            Op = op,
            Operand = reference.SubjectId,
            ResultType = "System.IntPtr",
            Callee = reference.SubjectId,
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "method",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeLdftnInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFunctionPointerInstruction(
            metadataReader,
            typeResolver,
            typeModels,
            methodOwners,
            "ldftn",
            ref ilReader);
        return instruction with { Op = "ldftn" };
    }

    private static ManagedInstructionModel DecodeLdvirtftnInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFunctionPointerInstruction(
            metadataReader,
            typeResolver,
            typeModels,
            methodOwners,
            "ldvirtftn",
            ref ilReader);
        return instruction with { Op = "ldvirtftn" };
    }

    private static ManagedInstructionModel DecodeFieldReferenceInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var reference = ResolveFieldReference(metadataReader, typeResolver, fieldOwners, handle);

        return new ManagedInstructionModel
        {
            Op = opCode switch
            {
                ILOpCode.Stfld => "stfld",
                ILOpCode.Ldfld => "ldfld",
                ILOpCode.Ldflda => "ldflda",
                ILOpCode.Stsfld => "stsfld",
                ILOpCode.Ldsfld => "ldsfld",
                ILOpCode.Ldsflda => "ldsflda",
                _ => throw new NotSupportedException($"unsupported field reference opcode: {opCode}"),
            },
            Operand = reference.SubjectId,
            ResultType = opCode switch
            {
                ILOpCode.Ldfld or ILOpCode.Ldsfld => reference.FieldType,
                ILOpCode.Ldflda or ILOpCode.Ldsflda => "System.IntPtr",
                _ => "System.Void",
            },
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "field",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeLdsfldInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFieldReferenceInstruction(
            metadataReader,
            typeResolver,
            fieldOwners,
            ILOpCode.Ldsfld,
            ref ilReader);
        return instruction with { Op = "ldsfld" };
    }

    private static ManagedInstructionModel DecodeStsfldInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFieldReferenceInstruction(
            metadataReader,
            typeResolver,
            fieldOwners,
            ILOpCode.Stsfld,
            ref ilReader);
        return instruction with { Op = "stsfld" };
    }

    private static ManagedInstructionModel DecodeLdargInstruction(MethodSummary methodSummary, int index)
    {
        string resultType;

        if (!methodSummary.IsStatic && index == 0)
        {
            resultType = methodSummary.DeclaringTypeSubjectId;
        }
        else
        {
            var parameterIndex = methodSummary.IsStatic ? index : index - 1;
            if (parameterIndex < 0 || parameterIndex >= methodSummary.Parameters.Count)
            {
                throw new InvalidOperationException($"invalid ldarg index {index} for method {methodSummary.SubjectId}");
            }

            resultType = methodSummary.Parameters[parameterIndex].Type;
        }

        return new ManagedInstructionModel
        {
            Op = "ldarg",
            Operand = index,
            ResultType = resultType,
        };
    }

    private static ManagedInstructionModel DecodeLdargaInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "ldarga",
            Operand = index,
            ResultType = "System.IntPtr",
        };
    }

    private static ManagedInstructionModel DecodeStargInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "starg",
            Operand = index,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeLdlocInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "ldloc",
            Operand = index,
        };
    }

    private static ManagedInstructionModel DecodeLdlocaInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "ldloca",
            Operand = index,
            ResultType = "System.IntPtr",
        };
    }

    private static ManagedInstructionModel DecodeStlocInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "stloc",
            Operand = index,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeLdcI4Instruction(int value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.i4",
            Operand = value,
            ResultType = "System.Int32",
        };
    }

    private static ManagedInstructionModel DecodeLdcI8Instruction(long value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.i8",
            Operand = value,
            ResultType = "System.Int64",
        };
    }

    private static ManagedInstructionModel DecodeLdcR4Instruction(float value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.r4",
            Operand = value,
            ResultType = "System.Single",
        };
    }

    private static ManagedInstructionModel DecodeLdcR8Instruction(double value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.r8",
            Operand = value,
            ResultType = "System.Double",
        };
    }

    private static ManagedInstructionModel DecodeSimpleInstruction(string op, string resultType)
    {
        return op switch
        {
            "add" => new ManagedInstructionModel { Op = "add", ResultType = resultType },
            "sub" => new ManagedInstructionModel { Op = "sub", ResultType = resultType },
            "mul" => new ManagedInstructionModel { Op = "mul", ResultType = resultType },
            "div" => new ManagedInstructionModel { Op = "div", ResultType = resultType },
            "rem" => new ManagedInstructionModel { Op = "rem", ResultType = resultType },
            "ceq" => new ManagedInstructionModel { Op = "ceq", ResultType = resultType },
            "clt" => new ManagedInstructionModel { Op = "clt", ResultType = resultType },
            "cgt" => new ManagedInstructionModel { Op = "cgt", ResultType = resultType },
            _ => new ManagedInstructionModel
            {
                Op = op,
                ResultType = resultType,
            },
        };
    }

    private static ManagedInstructionModel DecodeCalliInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MethodSummary methodSummary,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.Handle(token);
        if (handle.Kind != HandleKind.StandaloneSignature)
        {
            throw new NotSupportedException($"unsupported calli signature handle kind in loader: {handle.Kind}");
        }

        var signature = metadataReader
            .GetStandaloneSignature((StandaloneSignatureHandle)handle)
            .DecodeMethodSignature(typeResolver.TypeNameProvider, CreateSignatureContext(methodSummary));
        var parameterTypes = signature.ParameterTypes.ToArray();

        return new ManagedInstructionModel
        {
            Op = "calli",
            Operand = token,
            ResultType = signature.ReturnType,
            CallSiteSignature = new ManagedCallSiteSignature
            {
                KindCode = ManagedCallSiteKind.FunctionPointer,
                ReturnType = signature.ReturnType,
                ParameterTypes = parameterTypes,
            },
        };
    }

    private static SignatureContext<string>? CreateSignatureContext(MethodReferenceSummary methodSummary)
    {
        var typeArguments = methodSummary.Substitutions
            .Where(pair => pair.Key.StartsWith("!", StringComparison.Ordinal) && !pair.Key.StartsWith("!!", StringComparison.Ordinal))
            .OrderBy(pair => pair.Key, StringComparer.Ordinal)
            .Select(pair => pair.Value)
            .ToImmutableArray();
        var methodArguments = methodSummary.Substitutions
            .Where(pair => pair.Key.StartsWith("!!", StringComparison.Ordinal))
            .OrderBy(pair => pair.Key, StringComparer.Ordinal)
            .Select(pair => pair.Value)
            .ToImmutableArray();

        return typeArguments.IsDefaultOrEmpty && methodArguments.IsDefaultOrEmpty
            ? null
            : new SignatureContext<string>(typeArguments, methodArguments);
    }

    private static ManagedInstructionModel DecodeBranchInstruction(string op, int delta)
    {
        return new ManagedInstructionModel
        {
            Op = op,
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeLeaveInstruction(int delta)
    {
        return new ManagedInstructionModel
        {
            Op = "leave",
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeSwitchInstruction(ref BlobReader ilReader)
    {
        return new ManagedInstructionModel
        {
            Op = "switch",
            Operand = ReadSwitchTargets(ref ilReader),
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel? SkipConstrainedInstruction(ref BlobReader ilReader)
    {
        ilReader.ReadInt32();
        return null;
    }

    private static ManagedInstructionModel? SkipUnalignedInstruction(ref BlobReader ilReader)
    {
        ilReader.ReadByte();
        return null;
    }

    private static ManagedInstructionModel DecodeBrtrueInstruction(int delta)
    {
        return new ManagedInstructionModel
        {
            Op = "brtrue",
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeBrfalseInstruction(int delta)
    {
        return new ManagedInstructionModel
        {
            Op = "brfalse",
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static int ReadBranchTargetInt32(ref BlobReader ilReader)
    {
        var delta = ilReader.ReadInt32();
        return ilReader.Offset + delta;
    }

    private static int ReadBranchTargetSByte(ref BlobReader ilReader)
    {
        var delta = ilReader.ReadSByte();
        return ilReader.Offset + delta;
    }

    private static IReadOnlyList<int> ReadSwitchTargets(ref BlobReader ilReader)
    {
        var targetCount = ilReader.ReadInt32();
        var deltas = new int[targetCount];

        for (var index = 0; index < targetCount; index++)
        {
            deltas[index] = ilReader.ReadInt32();
        }

        var baseOffset = ilReader.Offset;
        var targets = new int[targetCount];
        for (var index = 0; index < targetCount; index++)
        {
            targets[index] = baseOffset + deltas[index];
        }

        return targets;
    }

    private static IReadOnlyList<ManagedExceptionRegionModel> DecodeExceptionRegions(
        MethodBodyBlock bodyBlock,
        MetadataTypeResolver typeResolver)
    {
        return bodyBlock.ExceptionRegions
            .Select(region => new ManagedExceptionRegionModel
            {
                HandlingKind = region.Kind switch
                {
                    ExceptionRegionKind.Catch => "catch",
                    ExceptionRegionKind.Finally => "finally",
                    ExceptionRegionKind.Fault => "fault",
                    ExceptionRegionKind.Filter => "filter",
                    _ => throw new NotSupportedException($"unsupported exception region kind in loader: {region.Kind}"),
                },
                TryOffset = region.TryOffset,
                TryLength = region.TryLength,
                HandlerOffset = region.HandlerOffset,
                HandlerLength = region.HandlerLength,
                FilterOffset = region.Kind == ExceptionRegionKind.Filter ? region.FilterOffset : null,
                CatchTypeSubjectId = region.Kind == ExceptionRegionKind.Catch && !region.CatchType.IsNil
                    ? typeResolver.ResolveTypeIdentity(region.CatchType).SubjectId
                    : null,
            })
            .ToList();
    }

    private static ILOpCode ReadOpCode(ref BlobReader ilReader)
    {
        var firstByte = ilReader.ReadByte();
        if (firstByte != 0xFE)
        {
            return (ILOpCode)firstByte;
        }

        return (ILOpCode)((firstByte << 8) | ilReader.ReadByte());
    }
}
