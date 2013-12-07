require 'spec_helper'

describe Digest::StringBuffer do
  it "initialize" do
    expect(Digest::StringBuffer.new).to be_a_kind_of(Digest::StringBuffer)
    expect(Test.new).to be_a_kind_of(Digest::StringBuffer)
  end

  it "digest" do
    expect(Test.digest("abc" * 1000)).to eq("\x00\x8B\x11\x90".force_encoding("ASCII-8BIT"))
  end

  it "hexdigest" do
    expect(Test.hexdigest("abc" * 1000)).to eq("008b1190")
  end

  it "update and reset and hexdigest" do
    test = Test.new
    test.update("b").update("u").update("ff")
    test << "er"
    expect(test.hexdigest).to eq("00004cc6");
    expect(test.hexdigest).to eq("00004cc6");
    expect(test.hexdigest!).to eq("00004cc6");
    expect(test.hexdigest).to eq("00000000");
  end

  it "==" do
    test1 = Test.new.update("test")
    test2 = Test.new.update("test")
    expect(test1).to eq(test2)
  end

  it "dup" do
    test1 = Test.new.update("buff")
    test2 = test1.dup.update("er")
    expect(test2.hexdigest).to eq("00004cc6");
  end

  it "length" do
    expect(Test.new.length).to eq(4);
  end

  it "buffer" do
    expect(Digest::StringBuffer.new.update("a").buffer).to eq("a")
    expect(Test.new.update("a").buffer).to eq("a")
  end
end
