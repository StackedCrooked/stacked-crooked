#!/usr/bin/ruby
require 'VideoConverter.rb'

if ARGV.empty?
  raise "Usage: ruby test.rb <videofile>"
end

# Remove previous output
puts "Remove previous output"
system("rm output.flv")

vc = VideoConverter.new(ARGV[0])
vc.convert_to("output.flv")


while not vc.done
  puts vc.get_progress.to_s + "%"
  sleep(1)
end

puts "Done!"



