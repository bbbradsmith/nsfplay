// This code has not been tested with multi-track nsf[e] files as I couldn't
// easily find any on the internet.
#include <iostream>
#include <string_view>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cassert>

#include <getopt.h>
#include <iconv.h>

#include "nlohmann/json.hpp"
#include "../xgm/xgm.h"


namespace {

using json = nlohmann::json;

std::string_view progname;

struct NsfMetaOptions {
  std::string encoding;
};

void Usage(std::ostream &output, int exit_code) {
    output
        << "Usage: " << progname << " [options] /path/to/nsf[e]" << std::endl
        << R"(Output metadata from an NSF[e] in JSON format.

The generated JSON will be an array of objects, one for each track. For example:

    [
        {
            "artist": "NARUTO",
            "copyright": "NARUTO",
            "title": "Artificial Intelligence Bomb"
        }
    ]

Options:
 -h, --help              Show this help message.
 -e, --encoding=UTF-8    The encoding of the metadata fetched from the NSF file.
)";
        std::exit(exit_code);
}

NsfMetaOptions ParseOptions(int *argc, char ***argv) {
    static constexpr struct option longopts[] = {
        { "help", no_argument, nullptr, 'h' },
        { "encoding", required_argument, nullptr, 'e' },
        { nullptr, 0, nullptr, 0 }
    };
    NsfMetaOptions options;
    int ch = 0;
    while ((ch = getopt_long(*argc, *argv, "he:", longopts, NULL)) != -1) {
        switch (ch) {
        case 'e':
            options.encoding = optarg;
            break;
        case 'h':
            Usage(std::cout, EXIT_SUCCESS);
        default:
            Usage(std::cerr, EXIT_FAILURE);
        }
    }
    *argc -= optind;
    *argv += optind;
    return options;
}

const iconv_t kFailedIconvT = reinterpret_cast<iconv_t>(-1);

class ToUTF8 {
public:
    ToUTF8() = default;

    ToUTF8(std::string_view input_encoding) {
        conv_ = iconv_open("UTF-8", input_encoding.data());
        if (conv_ == kFailedIconvT) {
            std::perror("iconv_open");
            std::exit(EXIT_FAILURE);
        }
    }

    ~ToUTF8() {
        if (conv_ == kFailedIconvT) return;
        if (iconv_close(conv_) == -1) {
            std::perror("iconv");
            std::exit(EXIT_FAILURE);
        }
    }

    ToUTF8(const ToUTF8 &) = delete;
    ToUTF8 &operator=(const ToUTF8 &) = delete;

    ToUTF8(ToUTF8 &&o) : ToUTF8() {
        swap(*this, o);
    }

    ToUTF8 &operator=(ToUTF8 &&o) {
        swap(*this, o);
        return *this;
    }

    friend void swap(ToUTF8 &a, ToUTF8 &b) {
        using std::swap;
        swap(a.conv_, b.conv_);
    }

    std::string Convert(std::string_view str) {
        std::string out;

        static constexpr size_t kBufferSize = BUFSIZ;

        char *inbuf = const_cast<char*>(str.data());
        size_t inbytesleft = str.length();
        char outbuf[kBufferSize];
        size_t outbytesleft = kBufferSize;
        char *outptr = outbuf;

        // Write out the byte sequence to get into the initial state (if
        // necessary).
        bool done = Convert(/*inbuf=*/nullptr, /*inbytesleft=*/nullptr, &outptr,
                            &outbytesleft);
        assert(done);

        do {
            done = Convert(&inbuf, &inbytesleft, &outptr, &outbytesleft);
            out.append(outbuf, kBufferSize - outbytesleft);
            outptr = outbuf;
            outbytesleft = kBufferSize;
        } while(!done);

        // Flush partially converted input.
        done = Convert(/*inbuf=*/nullptr, /*inbytesleft=*/nullptr, &outptr,
                       &outbytesleft);
        assert(done);

        return out;
    }

private:
    // Returns true when the input has been fully converted.
    bool Convert(char **inbuf, size_t *inbytesleft, char **outbuf,
                 size_t *outbytesleft) {
        size_t orig_ibl = 0;
        if (inbytesleft) orig_ibl = *inbytesleft;
        size_t ibl = orig_ibl;

        size_t orig_obl = 0;
        if (outbytesleft) orig_obl = *outbytesleft;
        size_t obl = orig_obl;

        assert(conv_ != kFailedIconvT);
        size_t nconv = iconv(conv_, inbuf, &ibl, outbuf, &obl);
        if (nconv == -1 && (errno != E2BIG || obl == orig_obl)) {
            std::perror("iconv");
            std::exit(EXIT_FAILURE);
        }

        if (outbytesleft) *outbytesleft = obl;
        if (inbytesleft) *inbytesleft = ibl;
        return ibl == 0;
    }

    iconv_t conv_ = kFailedIconvT;
};

}  // namespace

int main(int argc, char *argv[]) {
    progname = argv[0];
    NsfMetaOptions options = ParseOptions(&argc, &argv);

    if (argc != 1) Usage(std::cerr, EXIT_FAILURE);
    std::string_view nsf_path(argv[0]);

    ToUTF8 utf8(options.encoding);
    xgm::NSF nsf;

    if(!nsf.LoadFile(nsf_path.data())) {
        std::cerr << "Error loading NSF file '" << nsf_path << "': "
                  << nsf.LoadError() << std::endl;
        return EXIT_FAILURE;
    }

    json nsf_json = json::array();

    for (int track = 0; track < nsf.GetSongNum(); track++) {
      json &track_json = nsf_json[track] = json::object();
      nsf.SetSong(track);

      if (std::string_view title(nsf.title); !title.empty()) {
          track_json["title"] = utf8.Convert(title);
      }
      if (std::string_view artist(nsf.artist); !artist.empty()) {
          track_json["artist"] = utf8.Convert(artist);
      }
      if (std::string_view copyright(nsf.copyright); !copyright.empty()) {
          track_json["copyright"] = utf8.Convert(copyright);
      }
      if (std::string_view ripper(nsf.ripper); !ripper.empty()) {
          track_json["ripper"] = utf8.Convert(ripper);
      }
      if (std::string_view text(nsf.text, nsf.text_len); !text.empty()) {
          track_json["text"] = utf8.Convert(text);
      }
      if (int playtime = nsf.GetPlayTime(); playtime != nsf.default_playtime) {
          track_json["play_time_ms"] = playtime;
      }
      if (int fadetime = nsf.GetFadeTime(); fadetime != nsf.default_fadetime) {
          track_json["fade_ms"] = fadetime;
      }
      if (int loopnum = nsf.GetLoopNum(); loopnum != nsf.default_loopnum - 1) {
          track_json["loopnum"] = loopnum;
      }
      if (int looptime = nsf.GetLoopTime(); looptime != 0) {
          track_json["loop_time_ms"] = looptime;
      }
    }

    std::cout << nsf_json.dump(4) << std::endl;

    return EXIT_SUCCESS;
}
