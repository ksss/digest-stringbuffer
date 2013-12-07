begin
  require "digest/stringbuffer/#{RUBY_VERSION[/\d+.\d+/]}/stringbuffer"
rescue LoadError
  require "digest/stringbuffer/stringbuffer"
end
require "digest/stringbuffer/version"
