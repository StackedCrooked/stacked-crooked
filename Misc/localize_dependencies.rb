# Usage
# localize_dependencies.rb input_file output_directory

# Algorithm
# ---------
# Get list of input file dependencies
# Copy each dependency to output directory
# Update input_file paths to local dir (relative to @executable_path)
# Repeat above procedure for input file dependenc


require 'ftools'
require 'open3'

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


def needs_localization(file_path)
  return file_path.index("@executable_path") == nil &&
         file_path.index("/usr/lib/") == nil
end


def get_input_file_dependencies(input_file)
  result = []
  execute_and_handle("otool -L #{input_file}",
                     Proc.new { |outPipe|
                       outPipe.each_line do |line|
                         if line =~ /\s+(.+dylib)\s.+/
                           filename = $1
                           if (needs_localization(filename))
                             puts "Dependency found: #{$1}"
                             result.push($1)
                           end
                         end
                       end
                     },
                     nil)
  return result
end


def copy_dependencies(input_file, output_directory)
  File.makedirs(output_directory)
  get_input_file_dependencies(input_file).each do |dependency|
    target = File.join(output_directory, File.basename(dependency))
    if not File.exists?(target)
      File.copy(dependency, target)
      copy_dependencies(target, output_directory)
    end
    update_path(input_file, dependency, output_directory)
    update_id(target, output_directory)
  end
end


def update_internal_path(input_file, old_path, new_path)
  command = "install_name_tool -change #{old_path} #{new_path} #{input_file}"
  puts command
  execute_and_handle(command, nil, nil)
end


def update_path(input_file, cur_path, output_directory)
  update_internal_path(input_file,
                       cur_path,
                       "#{File.basename(output_directory)}/#{File.basename(cur_path)}")
end


def update_id(input_file, output_directory)
  command = "install_name_tool -id #{File.join(output_directory, File.basename(input_file))} #{input_file}"
  puts command
  execute_and_handle(command, nil, nil)
end

copy_dependencies(ARGV[0], ARGV[1])

