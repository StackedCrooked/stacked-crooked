require 'rubygems'
require 'fileutils'
require 'mongrel'
require 'popen4'


class SimpleHandler < Mongrel::HttpHandler
  def process(request, response)
    command = request.params["REQUEST_PATH"][1..-1].gsub(/%20/, " ").gsub(/%22/, "\"")
    puts "Received request: #{command}"

    # Empty command is replied with the home page.
    if command == ""
      response.start(200) do |head,out|
        head["Content-Type"] = "text/html"
        FileUtils.copy_stream(File.new("cmd.html"), out)
      end      
    # We don't return favicons.
    elsif command == "favicon.ico"
      # ignore this request

    # We accept the compile command and return the build output.
    elsif command == "compile"
        response.start(200) do |head,out|
            head["Content-Type"] = "text/html"
            File.open("dummy", "w") do |file|
                request.body.read(file)
            end

            out.write("Compilation succeeded.")
        end      
    end
  end
end


h = Mongrel::HttpServer.new("173.203.57.63", 4001)
h.register("/", SimpleHandler.new)
h.run.join
