require 'rubygems'
require 'fileutils'
require 'mongrel'
require 'popen4'


class SimpleHandler < Mongrel::HttpHandler
  def process(request, response)
	  command = request.params["REQUEST_PATH"][1..-1]
    puts "Command: " + command
    body = ""
    if command == ""
      response.start(200) do |head,out|
        head["Content-Type"] = "text/html"
        FileUtils.copy_stream(File.new("chat.html"), out)
      end      
    elsif command == "favicon.ico"
    elsif command == "output.html"
      puts "serving output.html"
      response.start(200) do |head,out|
        head["Content-Type"] = "text/html"
        FileUtils.copy_stream(File.new("output.html"), out)
      end
    else
      command = command.gsub(/%20/, " ").gsub(/%22/, "\"")
      puts "Sanitized command: " + command
      response.start(200) do |head,out|
        head["Content-Type"] = "text/html"
        status = POpen4::popen4(command) do |stdout, stderr, stdin, pid|
          stdin.close()
          FileUtils.copy_stream(stdout, out)
          FileUtils.copy_stream(stderr, out)
          puts "Sent response."
        end
      end
    end
  end
end

 host="localhost"
 port=4000
 h = Mongrel::HttpServer.new(host, port)
 h.register("/", SimpleHandler.new)
 h.run.join