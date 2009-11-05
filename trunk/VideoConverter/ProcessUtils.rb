require 'open3'

# Helper functions when running processes
module ProcessUtils

  # Runs a subprocess.
  # +command+:: command line string to be executed by the system
  # +outpipe+:: +Proc+ object that takes the stdout pipe (of type +IO+) as first and only parameter.
  # +errpipe+:: +Proc+ object that takes the stderr pipe (of type +IO+) as first and only parameter.
  def execute_and_handle(command, outpipe, errpipe)
    Open3.popen3(command) do |_, stdout, stderr|
      if (outpipe)
        outpipe.call(stdout)
      end
      if (errpipe)
        errpipe.call(stderr)
      end
    end
  end
end

