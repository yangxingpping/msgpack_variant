
#include "spdlog/spdlog.h"
#include "msgpack.hpp"
#include <variant>
#include <vector>
#include <string_view>
#include "ylt/struct_pack.hpp"
using std::vector;
using std::string_view;
// Define a variant type
using MyVariant = std::variant<int, std::string, double, vector<int>, vector<double>>;

// Specialize the MessagePack packer for MyVariant
namespace msgpack {
    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
        namespace adaptor {
            template <>
            struct pack<MyVariant> {
                template <typename Stream>
                msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MyVariant const& v) const {
                    o.pack_array(2);
                    o.pack(v.index());
                    std::visit([&o](auto&& arg) { o.pack(arg); }, v);
                    return o;
                }
            };

            template <>
            struct convert<MyVariant> {
                msgpack::object const& operator()(msgpack::object const& o, MyVariant& v) const {
                    if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) {
                        throw msgpack::type_error();
                    }
                    int index = o.via.array.ptr[0].as<int>();
                    switch (index) {
                    case 0:
                        v = o.via.array.ptr[1].as<int>();
                        break;
                    case 1:
                        v = o.via.array.ptr[1].as<std::string>();
                        break;
                    case 2:
                        v = o.via.array.ptr[1].as<double>();
                        break;
                    case 3:
                    {
                        vector<int> vv;
                        o.via.array.ptr[1].convert<vector<int>>(vv);
                        v = std::move(vv);
                    }break;
                    case 4:
                    {
                        vector<double> vv;
                        o.via.array.ptr[1].convert<vector<double>>(vv);
                        v = std::move(vv);
                    }break;
                    default:
                        throw msgpack::type_error();
                    }
                    return o;
                }
            };
        }
    }
}
int main(void)
{
    //// Create a variant
    //MyVariant original = vector<double>{1.0,2.0};

    //// Pack the variant
    //std::stringstream buffer;
    //msgpack::pack(buffer, original);

    //// Unpack the variant
    //std::string str = buffer.str();
    //msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
    //msgpack::object deserialized = oh.get();

    //// Convert the unpacked object to a variant
    //MyVariant result;
    //deserialized.convert(result);

    //// Display the result
    //std::visit([](auto&& arg)
    //{
    //        int j = 1;
    //        //SPDLOG_INFO("{}", arg);
    //}, result);
    MyVariant v = vector<double>{ 1.0,2.0 };
    auto result = struct_pack::serialize(v);
    MyVariant v2;
    auto d2 = struct_pack::deserialize<MyVariant>(string_view{result.data(), result.size()});
    auto d3 = d2.value();
	SPDLOG_INFO("hello,world");
	return 0;
}
