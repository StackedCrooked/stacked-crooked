require 'VideoConverter.rb'
require 'rubygems'
require 'sinatra'

$converter = VideoConverter.new
$mutex = Mutex.new
$cv = ConditionVariable.new


get '/convert' do
  Thread.new do 
    progress_handler = Proc.new do |progress|
      $mutex.synchronize do
        $progress = progress
        $cv.signal
      end
    end
    $converter.convert("mitch.mpg",
                       "mitch.avi",
                       progress_handler)
  end
  "Conversion progress has started, progress is now: " + $progress.to_s
end

get '/progress' do
  $mutex.synchronize do
    $cv.wait($mutex)
    "Progress is now: " + $progress.to_s
  end
end


