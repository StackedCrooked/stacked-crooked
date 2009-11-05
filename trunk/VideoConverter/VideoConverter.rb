require 'open3'

module ProcessUtils

  # Runs a subprocess and applies handlers for stdout and stderr
  def execute_and_handle(command, outhandler, errhandler)
    Open3.popen3(command) do |_, stdout, stderr|
      if (outhandler)
        outhandler.call(stdout)
      end
      if (errhandler)
        errhandler.call(stderr)
      end
    end
  end
end


class FFMPEG
  include ProcessUtils

  def initialize
    # Get the duration from ffmpeg's process output
    @duration_parser = Proc.new do |line|
      duration = 0
      if line =~ /Duration\: (\d+)\:(\d+)\:(\d+)\.(\d+)/
        duration = (($1.to_i * 60 + $2.to_i) * 60 + $3.to_i) * 10 + $4.to_i
      end
      duration
    end

    @progress_line_parser = Proc.new do |line|
      result = 0
      if line =~ /time=(\d+)\.(\d+)/ then
        result = ($1.to_i * 10 + $2.to_i) * 100
      end
      result
    end


    @progress_pipe_parser = Proc.new do |pipe|
      progress = 0
      pipe.each("\r") do |line|
        p = @progress_line_parser.call(line)
        if progress != p
          progress = p
          if @progress_handler
            @progress_handler.call(progress) if @progress_handler
          end
        end
      end
      $defout.flush
      progress
    end
  end

  def get_duration(file)
    duration = 0
    errhandler = Proc.new do |pipe|
      pipe.each("\n") do |line|
        # Get duration
        if (duration == 0)
          duration = @duration_parser.call(line)
        end
      end
    end
    execute_and_handle("ffmpeg -i #{file}", nil, errhandler)
    duration
  end

  # Execute a shell command as a subprocess and report progress
  # Params:
  # - command: command line
  # - progress_handler: lambda that takes number of seconds processed as first and only parameter
  #
  def convert(input_file, output_file, progress_handler)
    @progress_handler = progress_handler
    command = "ffmpeg -i #{input_file} #{output_file}"
    outhandler = nil
    execute_and_handle(command, nil, @progress_pipe_parser)
  end
end


#
# Configuration
#
$input_file = "./TestVideos/mitch.mpg"
$output_format = "flv"
$output_file = "output.#{$output_format}"

#
# Script
#
converter = FFMPEG.new


# Remove old output file
converter.execute_and_handle("rm #{$output_file}", nil, nil)

# Get duration
duration = converter.get_duration($input_file)

# Convert and report progress
converter.convert($input_file, $output_file, Proc.new { |progress| puts (progress/duration).to_s + "%" } )

# Done
puts "Done!"
