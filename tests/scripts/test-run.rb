#!/usr/bin/env ruby
# Runs one exact CLI fixture.
# .dat supplies stdin, .ans stdout, and optional .err/.status files describe
# stderr and the expected process exit status.

require 'open3'
require 'optparse'

options = { args: [], status: 0 }
OptionParser.new do |parser|
  parser.banner = 'Usage: test-run.rb --testfile APP --infile INPUT --etafile EXPECTED [--arg ARG]'
  parser.on('-t', '--testfile PATH') { |value| options[:testfile] = value }
  parser.on('-i', '--infile PATH') { |value| options[:infile] = value }
  parser.on('-e', '--etafile PATH') { |value| options[:etafile] = value }
  parser.on('--errfile PATH') { |value| options[:errfile] = value }
  parser.on('--statusfile PATH') { |value| options[:statusfile] = value }
  parser.on('-a', '--arg ARG') { |value| options[:args] << value }
end.parse!

required = %i[testfile infile etafile]
missing = required.reject { |key| options[key] }
abort "Missing required option(s): #{missing.join(', ')}" unless missing.empty?

required.each do |key|
  abort "File does not exist: #{options[key]}" unless File.file?(options[key])
end

if options[:errfile] && !File.file?(options[:errfile])
  abort "File does not exist: #{options[:errfile]}"
end
if options[:statusfile]
  abort "File does not exist: #{options[:statusfile]}" unless File.file?(options[:statusfile])
  options[:status] = Integer(File.read(options[:statusfile]).strip, 10)
end

input = File.binread(options[:infile])
expected = File.binread(options[:etafile])
expected_stderr = options[:errfile] ? File.binread(options[:errfile]) : ''
stdout, stderr, status = Open3.capture3(options[:testfile], *options[:args], stdin_data: input)

unless status.exitstatus == options[:status]
  warn stdout unless stdout.empty?
  warn stderr unless stderr.empty?
  abort "#{options[:testfile]} exited with status #{status.exitstatus}; expected #{options[:status]}"
end

unless stdout == expected
  warn "Output differs from #{options[:etafile]}"
  warn "--- expected\n#{expected}--- actual\n#{stdout}"
  exit 1
end

unless stderr == expected_stderr
  warn "Stderr differs from #{options[:errfile] || 'empty output'}"
  warn "--- expected stderr\n#{expected_stderr}--- actual stderr\n#{stderr}"
  exit 1
end
