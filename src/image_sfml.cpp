

#include "image_sfml.h"

ImageSFML::ImageSFML(const constants::mandelbuff &buff,
                     const std::shared_ptr<MandelParameters> &params,
                     const std::shared_ptr<Printer> &prnt,
                     std::tuple<int, int, int> rgb_base,
                     std::tuple<double, double, double> rgb_freq,
                     std::tuple<int, int, int> rgb_phase, uint8_t outfmt)
    : Imagewriter(buff, params, prnt),
      rgb_base(rgb_base),
      rgb_freq(rgb_freq),
      rgb_phase(rgb_phase),
      outfmt(outfmt)
{
}

ImageSFML::~ImageSFML() {}
void ImageSFML::write_buffer()
{
    std::vector<uint8_t> sfml_img_buf;
    // reserve memory this will make push_back less costly
    sfml_img_buf.reserve(this->params->xrange * this->params->yrange);
    // SFML needs a data structure with 4 uint8_t per RGBA pixel
    for (const auto &v : this->buff) {
        for (const auto &it : v) {
            // TODO: This code is quite similar to the one used in the ppm class
            unsigned int its = it.default_index;
            double continous_index = it.continous_index;
            if (its == this->params->bailout) {
                sfml_img_buf.push_back(0);
                sfml_img_buf.push_back(0);
                sfml_img_buf.push_back(0);
                sfml_img_buf.push_back(255);
                continue;
            }

            std::tuple<int, int, int> rgb;
            if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME) {
                rgb = this->rgb_linear(its, this->rgb_base, this->rgb_freq);
            }
            if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS) {
                rgb = this->rgb_continuous(continous_index, this->rgb_base,
                                           this->rgb_freq, this->rgb_phase);
            }
            sfml_img_buf.push_back(static_cast<uint8_t>(std::get<0>(rgb)));
            sfml_img_buf.push_back(static_cast<uint8_t>(std::get<1>(rgb)));
            sfml_img_buf.push_back(static_cast<uint8_t>(std::get<2>(rgb)));
            sfml_img_buf.push_back(255);
        }
    }

    std::string filename = this->out_file_name(
        this->params->image_base, this->params->bailout, this->params->xrange,
        this->params->yrange, this->params->zoom, this->params->cores,
        this->params->xcoord, this->params->ycoord, this->params->xl,
        this->params->xh, this->params->yl, this->params->yh);

    sf::Image img;
    img.create(this->params->xrange, this->params->yrange, sfml_img_buf.data());
    if ((outfmt & static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_JPG)) ==
        static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_JPG)) {
        this->prnt << "+ \u2937 " + filename + ".jpg" << std::endl;
        img.saveToFile(filename + ".jpg");
    }
    if ((outfmt & static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_PNG)) ==
        static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_PNG)) {
        this->prnt << "+ \u2937 " + filename + ".png" << std::endl;
        img.saveToFile(filename + ".png");
    }
}
