require 'open3'

# Helper functions when running processes
module ProcessUtils

  # Runs a subprocess and applies handlers for stdout and stderr.
  # +command+:: command line string to be executed by the system
  # +outhandler+:: +Proc+ object that takes the stdout pipe (of type +IO+) as first and only parameter. This value may be +nil+ if you don't want to process the standard output stream.
  # +errhandler+:: +Proc+ object that takes the stderr pipe (of type +IO+) as first and only parameter. This value may be +nil+ if you don't want to process the standard error stream.
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

