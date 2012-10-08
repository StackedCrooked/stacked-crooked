require 'rubygems'
require 'fileutils'
require 'mongrel'
require 'popen4'
require 'pp'


class SimpleHandler < Mongrel::HttpHandler
    def process(request, response)
        response.start(200) do |head,out|
            head["Content-Type"] = "text/html"
            case get_location(request)
            when ""
                FileUtils.copy_stream(File.new("cmd.html"), out)
            when "compile"

                # WRITE MAIN
                File.open("main.cpp", 'w') { |f| f.write(request.body.string) }
                puts "Verify main: #{File.readlines("main.cpp") { |f| puts f }}"

                # COMPILE
                status = POpen4::popen4("/usr/bin/g++ -o test main.cpp >output 2>&1") do |stdout, stderr, stdin, pid|
                    stdin.close()
                end

                puts "Verify main: #{File.readlines("output") { |f| puts f }}"

                if status == 0
                    out << "Compilation succeeded.\n\n"
                else
                    out << "Compilation failed with the following errors:\n\n"
                end
                FileUtils.copy_stream(File.new("output"), out)
                puts "Done"
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
