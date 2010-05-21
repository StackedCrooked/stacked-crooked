require 'VideoConverter.rb'
require 'rubygems'
require 'sinatra'

$in_video = "mitch.mpg"
$out_video = "mitch.avi"
$converter = VideoConverter.new($in_video)

# Todo: 'convert' is not a GET request!! Fix it!!
get '/convert' do
  $converter.convert_to($out_video)
  "Conversion progress has started"
end


get '/progress' do
  "Progress is now: " + $converter.get_progress.to_s
end

