# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'digest/stringbuffer/version'

Gem::Specification.new do |spec|
  spec.name          = "digest-stringbuffer"
  spec.version       = Digest::StringBuffer::VERSION
  spec.author        = "ksss"
  spec.email         = "co000ri@gmail.com"
  spec.description   = %q{Digest::StringBuffer is a class that subset of Digest::Instance.}
  spec.summary       = %q{Digest::StringBuffer is a class that subset of Digest::Instance.}
  spec.homepage      = ""
  spec.license       = "MIT"

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]
  spec.extensions    = ["ext/digest/stringbuffer/extconf.rb"]

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rspec", ['~> 2.11']
  spec.add_development_dependency "rake-compiler", ["~> 0.8.3"]
end
