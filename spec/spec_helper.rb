require 'digest/stringbuffer'

class Test < Digest::StringBuffer
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
