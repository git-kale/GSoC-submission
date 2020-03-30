#include <hpx/hpx_init.hpp>
#include <hpx/include/future.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/parallel_for_each.hpp>
#include <hpx/program_options.hpp>

#include <boost/range/counting_range.hpp>
#include <boost/range/irange.hpp>

#include <vector>
#include <algorithm>

using grid_f = std::vector<hpx::shared_future<std::vector<std::size_t> > >;
using grid = std::vector<std::vector<std::size_t> >;
using line = std::vector<std::size_t>;

line multiply (line m1, line m2)
{
    line mf(m1.size());
    for (const auto& y: boost::counting_range(0ul, m1.size()))
    {
            mf[y] = 0;
            for (const auto& k: boost::counting_range(0ul, m2.size()))
                mf[y] += m1[k] * m2[k];
    }

    return mf;
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    // Get commandline arguments
    std::size_t rows = vm["rows"].as<std::size_t>();
    std::size_t cols = vm["cols"].as<std::size_t>();

    grid_f m1;
    m1.resize(rows);

    // Store transpose of m2 for faster computation
    grid_f m2;
    m2.resize(rows);

    grid_f mf;
    mf.resize(rows);

    auto range = boost::irange(0ul, rows);
    hpx::parallel::for_each(
                hpx::parallel::execution::par,
                std::begin(range),
                std::end(range),
                [cols, &m1, &m2](std::size_t i)
                {
                    // // initialize (col, row) -> col
                    m1[i] =
                        hpx::make_ready_future(line(cols, i));
                    m2[i] =
                        hpx::make_ready_future(line(cols, i));
                }
    );

    auto Op = hpx::util::unwrapping(multiply);

    for (const auto& row: boost::counting_range(0ul, rows))
    {
        mf[row] = hpx::dataflow(Op, m1[row], m2[row]);
    }

    hpx::wait_all(mf);

    grid result = hpx::util::unwrap(mf);

    // // Print f
    for (const auto& y: result)
    {
        for (const auto& x: y)
            std::cout << x << " ";
        std::cout << std::endl;
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description commandline;
    commandline.add_options()
        ("cols", value<std::size_t>()->default_value(3),
         "Number of columns")
        ("rows", value<std::size_t>()->default_value(3),
         "Number of rows")
    ;

    return hpx::init(commandline, argc, argv);
}
