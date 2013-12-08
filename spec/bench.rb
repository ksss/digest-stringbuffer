#! /usr/bin/env ruby

lib = File.expand_path('../../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

require 'digest/stringbuffer'
require 'benchmark'

module Digest
  class BenchClass < Digest::Class
    def initialize(*arg)
      super
      @buf = []
    end

    def update (arg)
      @buf << arg
      self
    end

    def finish
      (@buf.join('').length % 256).chr
    end

    def reset
      @buf.clear
      self
    end
  end

  class BenchBuffer < Digest::StringBuffer
    def finish
      (buffer.length % 256).chr
    end
  end
end

Benchmark.bm do |x|
  x.report("Digest::Class long string") do
    Digest::BenchClass.digest("abc" * 1_000_000)
  end

  x.report("Digest::StringBuffer long string") do
    Digest::BenchBuffer.digest("abc" * 1_000_000)
  end

  x.report("Digest::Class meny times") do
    1000_000.times {
      Digest::BenchClass.digest("abc")
    }
  end

  x.report("Digest::StringBuffer meny times") do
    1000_000.times {
      Digest::BenchBuffer.digest("abc")
    }
  end
end
