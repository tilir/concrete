#!/usr/bin/ruby
# -----------------------------------------------------------------------------
#
# Test runner
#
# Runs test with specified input and compares output with expected output
#
# run: test-run.rb --testfile=test.x --infile=001.dat --etafile=001.ans
#
# -----------------------------------------------------------------------------

require 'open3'
require 'optparse'
require 'tempfile'

class TestRunner 
  def initialize(testf, inf, etaf)
    @testfile = testf
    @infile = inf
    @etafile = etaf

    if not File.file?(testf)
      puts "Cannot find program under test: #{testf}"
      exit 1
    end
    if not File.file?(inf)
      puts "Cannot find input file: #{inf}"
      exit 1
    end
    if not File.file?(inf)
      puts "Cannot find etalon file: #{etaf}"
      exit 1
    end
  end

  def run
    tfile = Tempfile.new
    tfile.close
    result = system(%("#{@testfile}" < "#{@infile}" > "#{tfile.path}"))
    if result != true
      p "command <#{@testfile} < #{@infile} > #{tfile.path}> failed"
      exit 1
    end
    result = system(%("diff" -s -w "#{@etafile}" "#{tfile.path}" > /dev/null))
    if result.nil?
      p "command <diff -s -w #{@etafile} #{tfile.path} > /dev/null> failed"
      exit 1
    end
    if result != true
      # need to materialize temp file here
      permpath = "#{File.basename(@etafile,'.*')}.out"
      FileUtils.cp(tfile.path, permpath)
      p "files #{@etafile} and #{permpath} differ"
      exit 1
    end
    tfile.unlink
  end
end


def parse_options
  options = {}

  OptionParser.new do |opts|
    opts.banner = "Usage: test-run.rb [options]"

    opts.on("-i", "--infile PATH", "Full path to input file. Mandatory argument. No default value.") do |v|
      options[:infile] = v
    end

    opts.on("-t", "--testfile PATH", "Full path to program under test.") do |v|
      options[:testfile] = v
    end

    opts.on("-e", "--etafile PATH", "Full path to etalon result.") do |v|
      options[:etafile] = v
    end

    opts.on("-v", "--[no-]verbose", "Run verbosely. No default value.") do |v|
      options[:verbose] = v
    end

    opts.on("-h", "--help", "Prints this help.") do
      puts opts
      exit
    end
  end.parse!

  if options[:infile] == nil or options[:testfile] == nil or options[:etafile] == nil
    puts "You need to specify --infile, --etafile and --testfile to run test"
    exit 1
  end

  p options if options[:verbose]
  options
end

# --------------------------------- Main program -------------------------------

def main
  options = parse_options
  testrun = TestRunner.new(options[:testfile], options[:infile], options[:etafile])
  testrun.run  
end

main
