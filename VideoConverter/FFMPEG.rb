require 'ProcessUtils.rb'


# Provides an interface to the +ffmpeg+ binary.
class FFMPEG
  include ProcessUtils

  def initialize
    @ffmpeg_executable = "ffmpeg"
  end

  # Tries to extract progress information from a line of ffmpeg's output.
  # +line+::  ffmpeg output line
  def parse_progress_line(line)
    result = 0
    if line =~ /time=(\d+)\.(\d+)/ then
      result = ((100.0 * $1.to_f) + $2.to_f) / 100.0
    end
    result
  end


  # Tries to extract duration information from a line of ffmpeg's output
  # Returns seconds.
  # +line+:: ffmpeg output line
  def parse_duration(line)
    duration = 0
    if line =~ /Duration\: (\d+)\:(\d+)\:(\d+)\.(\d+)/
      duration = ((($1.to_f * 60.0 + $2.to_f) * 60.0 + $3.to_f) * 10.0 + $4.to_f) / 10.0
    end
    duration
  end


  # Returns the duration in seconds for a given video file.
  # It does this by calling ffmpeg and parsing the output from the stderr channel.
  # +file+:: Path to the media file
  def get_duration(file)
    duration = 0
    errhandler = Proc.new do |pipe|
      pipe.each("\n") do |line|
        # Get duration
        if (duration == 0)
          duration = parse_duration(line)
        end
      end
    end
    execute_and_handle("ffmpeg -i #{file}", nil, errhandler)
    duration
  end

  # Executes a simple ffmpeg convert command of the form <tt>ffmpeg -i <inputfile> <outputfile></tt>
  # +input_file+:: path to the input video file
  # +output_file+:: path for the output video file
  # +progress_handler+:: +Proc+ object that takes a progress value (in seconds).
  def convert(input_file, output_file, progress_handler)
    @progress_handler = progress_handler
    command = "#{@ffmpeg_executable} -i #{input_file} #{output_file}"
    execute_and_handle(command, nil, Proc.new do |pipe|
      progress = 0
      pipe.each("\r") do |line|
        p = parse_progress_line(line)
        if progress <= p
          progress = p
          @progress_handler.call(progress) if @progress_handler
        end
      end
      $defout.flush
      progress
    end)
  end
end

