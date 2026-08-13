#!/usr/bin/env ruby
# Runs a real producer-to-consumer process pipeline and compares its stdout
# byte-for-byte with the supplied answer file.

require 'open3'
require 'optparse'

options = { producer_args: [], consumer_args: [] }
OptionParser.new do |parser|
  parser.on('--producer PATH') { |value| options[:producer] = value }
  parser.on('--producer-arg ARG') { |value| options[:producer_args] << value }
  parser.on('--consumer PATH') { |value| options[:consumer] = value }
  parser.on('--consumer-arg ARG') { |value| options[:consumer_args] << value }
  parser.on('--infile PATH') { |value| options[:infile] = value }
  parser.on('--etafile PATH') { |value| options[:etafile] = value }
end.parse!

required = %i[producer consumer infile etafile]
missing = required.reject { |key| options[key] }
abort "Missing required option(s): #{missing.join(', ')}" unless missing.empty?

input = File.binread(options[:infile])
expected = File.binread(options[:etafile])
commands = [
  [options[:producer], *options[:producer_args]],
  [options[:consumer], *options[:consumer_args]]
]

stdin, stdout, wait_threads = Open3.pipeline_rw(*commands)
stdin.write(input)
stdin.close
actual = stdout.read
stdout.close
statuses = wait_threads.map(&:value)

unless statuses.all?(&:success?)
  abort "Pipeline failed with statuses: #{statuses.map(&:exitstatus).join(', ')}"
end

unless actual == expected
  warn "Pipeline output differs from #{options[:etafile]}"
  warn "--- expected\n#{expected}--- actual\n#{actual}"
  exit 1
end
