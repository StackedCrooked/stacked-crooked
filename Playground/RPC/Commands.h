#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"


/**
 * RPC_CALL macro
 *
 * Defines a new Remote Procedure Call.
 *
 * The "RPC_CALL" macro takes two arguments: the function name and signature.
 * A simple integer increment function could be defined and used as follows:
 *
 *   // Declare (required for both client and server)
 *   RPC_CALL(Increment, int(int))
 *
 *   // Implemement (only required for server)
 *   int Increment::Implement(int value)
 *   {
 *       return value + 1;
 *   }
 *
 *   // Usage (only available for client code)
 *   int result = Increment(3).send();
 *
 * Serialization and de-serialization for the return and parameter values is
 * taken care of under the hood. It works out-of-the box for builtin types,
 * standard containers, boost tuple types and any combination of these.
 * It does not work out-of-the-box for user classes and structs. You can make
 * them serializable using the boost serialization library. (See the docs for
 * more info.)
 *
 * Multiple parameters can be passed if bundled in a tuple or standard container.
 * Passing a struct requires you to take care of serialization.
 * A nice solution is to use a typedef to a tuple object. For example:
 *
 * // Define the parameter type: bundle the two arguments in tuple.
 * typedef tuple<double, double> two_doubles;
 *
 * // Define the RPC call. You can think of it as a function signature:
 * // double Multiply(double, double);
 * RPC_CALL(Multiply, double(two_doubles));
 *
 * // The macro generated a declaration to the Multiply::Implement method.
 * // It's up to you to write the implementation. (If you don't do this then
 * // you'll get linker errors.)
 * double Multiply::Implement(const two_doubles & arg)
 * {
 *     return arg.get<0>() * arg.get<1>();
 * }
 *
 * // Try it out
 * // Single call:
 * double result = Multiply(two_doubles(1.1, 2.3)).send();
 *
 * // Batch call:
 * std::vector<two_doubles> numbers;
 * numbers.push_back(two_doubles(1.0, 2.0));
 * numbers.push_back(two_doubles(3.0, 4.0));
 * numbers.push_back(two_doubles(5.0, 6.0));
 * std::vector<double> results = Batch<Multiply>(numbers).send();
 */
RPC_CALL(CreateStopwatch, RemoteStopwatch(std::string))
RPC_CALL(StartStopwatch, Void(RemoteStopwatch))
RPC_CALL(CheckStopwatch, unsigned(RemoteStopwatch))
RPC_CALL(StopStopwatch, unsigned(RemoteStopwatch))
RPC_CALL(DestroyStopwatch, Void(RemoteStopwatch))


#endif // RPC_COMMANDS_H
