require 'spec_helper'

describe Digest::StringBuffer do
  it "update nil" do
    expect{ Digest::StringBuffer.new.update }.to raise_error(ArgumentError)
    expect{ Test.new.update }.to raise_error(ArgumentError)
  end

  it "no override digest" do
    expect{ Digest::StringBuffer.digest("a") }.to raise_error(RuntimeError)
  end
end



