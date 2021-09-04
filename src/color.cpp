#include "color.h"

void write_color(std::ostream& out, color pixel_color, const int samples_per_pixel) {
    auto [r, g, b] = pixel_color.e;

    auto scale = 1.0 / samples_per_pixel;

    r = sqrt(r * scale);
    g = sqrt(g * scale);
    b = sqrt(b * scale);

    static const double coeff = (double)(1 << color_bits) - 0.000001;
        out
            << static_cast<int>(coeff * clamp(r, 0, 0.999999)) << ' '
            << static_cast<int>(coeff * clamp(g, 0, 0.999999)) << ' '
            << static_cast<int>(coeff * clamp(b, 0, 0.999999)) << '\n';
}

void init_stream_picture(std::ostream& out) {
    out << "P3\n"
        << image_width << ' ' << image_height << '\n'
        << (1 << color_bits) << '\n';
}