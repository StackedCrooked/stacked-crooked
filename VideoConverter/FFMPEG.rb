require 'ProcessUtils.rb'

class FFMPEG
  include ProcessUtils

  # Tries to extract progress information from a line of FFMPEG output
  def parse_progress_line(line)
    result = 0
    if line =~ /time=(\d+)\.(\d+)/ then
      result = ($1.to_i * 10 + $2.to_i) * 100
    end
    result
  end


  # Tries to extract duration information from a line of FFMPEG output
  def parse_duration(line)
    duration = 0
    if line =~ /Duration\: (\d+)\:(\d+)\:(\d+)\.(\d+)/
      duration = (($1.to_i * 60 + $2.to_i) * 60 + $3.to_i) * 10 + $4.to_i
    end
    duration
  end


  # Returns the duration in seconds for a given video file.
  # It does this by calling FFMPEG and parsing the output from the stderr channel.
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

  # Execute a shell command as a subprocess and report progress
  # Params:
  # - command: command line
  # - progress_handler: proc object that takes number of seconds
  #                     processed as first and only parameter
  def convert(input_file, output_file, progress_handler)
    @progress_handler = progress_handler
    command = "ffmpeg -i #{input_file} #{output_file}"
    outhandler = nil
    execute_and_handle(command, nil, Proc.new do |pipe|
      progress = 0
      pipe.each("\r") do |line|
        p = parse_progress_line(line)
        if progress != p
          progress = p
          @progress_handler.call(progress) if @progress_handler
        end
      end
      $defout.flush
      progress
    end)
  end
end

