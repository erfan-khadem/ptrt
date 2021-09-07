#include "color.h"

void write_color(
		std::ostream& out,
		color pixel_color,
		const int samples_per_pixel,
		const scene::scene_description& sd
){
    static const double coeff = (double)(1 << sd.render.color_bits) - 0.000001;

    auto [r, g, b] = pixel_color.e;
    auto scale = 1.0 / samples_per_pixel;

    r = sqrt(r * scale);
    g = sqrt(g * scale);
    b = sqrt(b * scale);
    out
            << static_cast<int>(coeff * clamp(r, 0, 0.999999)) << ' '
            << static_cast<int>(coeff * clamp(g, 0, 0.999999)) << ' '
            << static_cast<int>(coeff * clamp(b, 0, 0.999999)) << '\n';
}

void init_stream_picture(
		std::ostream& out,
		const scene::scene_description& sd
){
    out << "P3\n"
        << sd.render.image_width << ' ' << sd.render.image_height << '\n'
        << (1 << sd.render.color_bits) << '\n';
}
