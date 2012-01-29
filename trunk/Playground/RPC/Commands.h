#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"


/**
 * RPC_CALL macro for defining remote procedure calls.
 *
 * Technically the macro only takes two arguments: name and signature:
 *
 *     RPC_CALL(Name, Signature)
 *
 * The signature always contains a return value and exactly one argument.
 * If you wan't multiple arguments you can use a struct, tuple or vector.
 * If want to return `void` then you can `return Void();`.
 *
 * A simple `Add` function for adding two numbers could be defined as:
 *
 *     // -> Declare call with a name and signature.
 *     typedef std::pair<int, int> Pair;
 *     RPC_CALL(Add, int(Pair)
 *
 *     // -> A helper class with the given name is generated.
 *     //    You need to provide an implementation for its `execute` method:
 *     int Add::execute(const Pair & value)
 *     {
 *         return value.first + value.second;
 *     }
 *
 *     // -> You can now use it as follows:
 *     int seven = Add(make_pair(3, 4)).send();
 *
 *     // -> There is also the `Batch` utility that allows to apply same
 *     //    method on multiple values:
 *     std::vector<Pair> pairs = get_some_pairs();
 *     std::vector<int> sums = Batch<Add>(pairs).send();
 *
 * Serialization works out-of-the box for builtin types, most standard containers,
 * boost tuple types and any combination of these. User defined structs and
 * classes must be made serializable by providing a `serialize` function.
 * See the boost serialization documentation for more info.
 */
RPC_CALL(CreateStopwatch, RemoteStopwatch(std::string))
RPC_CALL(StartStopwatch, Void(RemoteStopwatch))
RPC_CALL(CheckStopwatch, unsigned(RemoteStopwatch))
RPC_CALL(StopStopwatch, unsigned(RemoteStopwatch))
RPC_CALL(DestroyStopwatch, Void(RemoteStopwatch))


#endif // RPC_COMMANDS_H
