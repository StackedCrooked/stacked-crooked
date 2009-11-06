require 'VideoConverter.rb'
require 'rubygems'
require 'sinatra'

$converter = VideoConverter.new

get '/convert' do
  Thread.new do 
    $converter.convert("mitch.mpg", "mitch.avi")
    "Conversion progress has started, progress is now: " + $converter.get_progress().to_s
  end
end

get '/progress' do
  "Progress is now: " + $converter.get_progress().to_s
end


