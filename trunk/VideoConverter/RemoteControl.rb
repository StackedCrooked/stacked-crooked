require 'VideoConverter.rb'
require 'rubygems'
require 'sinatra'

$in_video = "mitch.mpg"
$out_video = "mitch.avi"
$converter = VideoConverter.new($in_video)

get '/convert' do
  $converter.convert_to($out_video)
  "Conversion progress has started"
end

get '/progress' do
  "Progress is now: " + $converter.get_progress($in_video)
end


