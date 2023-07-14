//
// Created by Yannic Lieder on 2019-08-31.
//

#ifndef MWT_PROGRESS_BAR_H
#define MWT_PROGRESS_BAR_H

namespace utils {
    class ProgressBar {
    public:
        ProgressBar(int max_iteration, bool percentage_only = false, bool new_line = true)
        {
            this->max_iteration = max_iteration;
            this->percentage_only = percentage_only;
            this->new_line = new_line;
            this->iteration = 0;
            this->percentage = 0;

            if (percentage_only) {
                std::cout << "[  0%]";
            }
            else
            {
                std::cout << "0%";
            }
            std::flush(std::cout);
        }

        ProgressBar& operator++() // Prefix increment
        {
            ++iteration;

            if (percentage < 100 && max_iteration >= 100 && iteration % int(round(max_iteration / 100)) == 0)
            {
                ++percentage;
                std::cout << "\b\b\b";

                if (percentage_only)
                {
                    if (percentage > 9) {
                        if (percentage == 100) {
                            std::cout << "\b";
                        }
                        std::cout << "\b";
                    }

                    std::cout << percentage << "%]";
                }
                else
                {

                    if (percentage > 10) {
                        std::cout << "\b";
                    }

                    std::cout << "= " << percentage << "%";
                }

                std::flush(std::cout);

                if (percentage == 100 && new_line) {
                    std::cout << std::endl;
                }
            }

            return *this;
        }

        ProgressBar operator++(int) // Postfix increment
        {
            ProgressBar tmp(*this);
            operator++();
            return tmp;
        }

    private:
        int percentage;
        int iteration;
        int max_iteration;
        bool percentage_only;
        bool new_line;
    };
}

#endif //MWT_PROGRESS_BAR_H
