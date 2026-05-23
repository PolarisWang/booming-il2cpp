namespace Chaos.IL2CPP.Contracts;

/// <summary>
/// Represents the result of a pipeline stage operation.
/// Use IsSuccess/IsFailure to check outcome, then access Value or Error.
/// </summary>
public sealed record PipelineResult<T>
{
    public T? Value { get; init; }
    public PipelineError? Error { get; init; }

    public bool IsSuccess => Error is null;
    public bool IsFailure => Error is not null;

    public T GetValueOrThrow()
    {
        if (IsFailure)
            throw new InvalidOperationException($"Pipeline failed: {Error!.Code} — {Error.Message}", Error.Exception);
        return Value!;
    }

    public static PipelineResult<T> Ok(T value) => new() { Value = value };
    public static PipelineResult<T> Fail(PipelineError error) => new() { Error = error };
    public static PipelineResult<T> Fail(string code, string message, Exception? exception = null) =>
        new() { Error = new PipelineError { Code = code, Message = message, Exception = exception } };
}

/// <summary>
/// Describes a pipeline failure with a machine-readable code and human-readable message.
/// </summary>
public sealed record PipelineError
{
    public required string Code { get; init; }
    public required string Message { get; init; }
    public Exception? Exception { get; init; }
}
