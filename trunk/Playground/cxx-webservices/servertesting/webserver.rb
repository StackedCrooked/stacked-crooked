require 'rubygems'
require 'fileutils'
require 'mongrel'
require 'popen4'

# Runs a subprocess and applies handlers for stdout and stderr
# Params:
# - command: command line string to be executed by the system
# - outhandler: proc object that takes a pipe object as first and only param (may be nil)
# - errhandler: proc object that takes a pipe object as first and only param (may be nil)
def execute_and_handle(command, outhandler, errhandler)
    Open3.popen3(command) do |_, stdout, stderr|
        if (outhandler)
            outhandler.call(stdout)
        end
        if (errhandler)
            errhandler.call(stderr)
        end
    end
end


class SimpleHandler < Mongrel::HttpHandler
    def process(request, response)
        response.start(200) do |head,out|
            head["Content-Type"] = "text/html"
            case get_location(request)
            when ""
                out.write("hi")
            when "compile"
                status = POpen4::popen4("g++ -o test main.cpp >output 2>&1") do |stdout, stderr, stdin, pid|
                    stdin.close()
                end
                FileUtils.copy_stream(File.new("output"), out)
                puts "Sent response"
            when "favicon.ico"
                # Don't respond to favicon..
            else
                puts "Don't know how to respond to the request to #{location}."
            end
        end
    end


    # Returns the location. E.g: if the URL is "http://localhost.com/compile" then "ccompile" will be returned.
    def get_location(request)
        return request.params["REQUEST_PATH"][1..-1].gsub(/%20/, " ").gsub(/%22/, "\"")
    end

    def return_compilation_result(out, payload)
    end

end


#host = "173.203.57.63"
host = "localhost"

port = 4000
#port = (9000 + 1000 * rand).round


puts "Listening to port #{port}"
h = Mongrel::HttpServer.new(host, port)
h.register("/", SimpleHandler.new)
h.run.join
