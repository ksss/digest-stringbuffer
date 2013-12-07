require 'spec_helper'

describe Digest::StringBuffer do
  it "gc safe" do
    test = Test.new
    GC.start
    test.update("buff")
    GC.start
    expect(test.update("er").to_s).to eq("00004cc6");
  end
end




