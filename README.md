# Digest::StringBuffer

[![Build Status](https://travis-ci.org/ksss/digest-stringbuffer.png?branch=master)](https://travis-ci.org/ksss/digest-stringbuffer)

**Digest::StringBuffer** is a class that just only to string buffering for message digest.

**Digest::StringBuffer** increase speed by limiting function.

**Digest::StringBuffer** more fast than **Digest::Class** when calculate hash meny times.

## Usage

Digest::StringBuffer depends only one method `finish`.

```ruby
module Digest
  class Prime31 < StringBuffer
    def initialize
      @prime = 31
    end
  
    def finish
      result = 0
      buffer.unpack("C*").each do |c|
        result += (c * @prime)
      end
      [result & 0xffffffff].pack("N")
    end
  end
end

p Digest::Prime31.hexdigest("abc" * 1000) #=> "008b1190"
```

## APIs

### Class methods

`digest`: make self instance and execute `update` and `digest`

`hexdigest`: make self instance and execute `update` and `hexdigest`

### Instance methods

`update`: add string in buffer.

`<<`: alias of `update`.

`finish`: should be overriden subclasses.

`reset`: initialize instance

`digest`: execute `finish`. it's should be return string.

`digest!`: execute `digest` and `reset`

`hexdigest`: execute `digest` and change to hex string.

`hexdigest!`: execute `hexdigest` and `reset`

`digest_length`: byte size of `digest`

`length`: and `size` alias of `digest`

`==`: check same string when execute `digest`

`initialize_copy`: run when `dup` and `clone`

`to_s`: same mean `hexdigest` when no arguments

`buffer`: string of all buffering now.

## Installation

Add this line to your application's Gemfile:

    gem 'digest-stringbuffer'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install digest-stringbuffer

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
