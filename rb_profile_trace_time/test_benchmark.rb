require "benchmark"

class A
  def to_string
    sleep(0.00004)
  end
end

a = A.new

Benchmark.bm do |x|
  x.report do
    1_00_000.times { a.to_string }
  end
end
