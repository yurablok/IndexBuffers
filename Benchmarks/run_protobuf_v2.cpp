#include "run_protobuf.h"
#include "protobuf/addressbook_v2.pb.h"


float run_protobuf_v2(const uint32_t reps) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    volatile int32_t sum = 0;
    uint8_t buffer[512];
    const auto begin = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < reps; ++i) {
        proto_v2::AddressBook addressBookSrc;
        {
            auto alice = addressBookSrc.add_people();
            alice->set_id(123);
            alice->set_name("Alice");
            alice->set_email("alice@example.com");
            auto alicePhones = alice->add_phones();
            alicePhones->set_number("555-1212");
            alicePhones->set_type(proto_v2::Person_PhoneType_MOBILE);
            //alice->employment school MIT
            alice->set_school("MIT");

            auto bob = addressBookSrc.add_people();
            bob->set_id(456);
            bob->set_name("Bob");
            bob->set_email("bob@example.com");
            auto bobPhones = bob->add_phones();
            bobPhones->set_number("555-4567");
            bobPhones->set_type(proto_v2::Person_PhoneType_HOME);
            bobPhones = bob->add_phones();
            bobPhones->set_number("555-7654");
            bobPhones->set_type(proto_v2::Person_PhoneType_WORK);
            //bob->employment unemployed
            bob->set_unemployed(true);
            addressBookSrc.SerializeToArray(buffer, sizeof(buffer));
        }
        const uint32_t size = addressBookSrc.ByteSizeLong(); // 106
        proto_v2::AddressBook addressBookDst;
        {
            addressBookDst.ParseFromArray(buffer, addressBookSrc.ByteSizeLong());
            for (uint8_t personIt = 0, personSize = addressBookDst.people_size();
                    personIt < personSize; ++personIt) {
                auto& person = addressBookDst.people(personIt);
                sum += person.name().size();
                for (uint8_t phoneIt = 0, phoneSize = person.phones_size();
                        phoneIt < phoneSize; ++phoneIt) {
                    auto& phone = person.phones(phoneIt);
                    sum += phone.type();
                    sum += phone.number().size();
                }
                switch (person.employment_case()) {
                case proto_v2::Person::EmploymentCase::kUnemployed:
                    sum += 'unem';
                    break;
                case proto_v2::Person::EmploymentCase::kEmployer:
                    sum += 'empl';
                    sum += person.employer().size();
                    break;
                case proto_v2::Person::EmploymentCase::kSchool:
                    sum += 'scho';
                    sum += person.school().size();
                    break;
                case proto_v2::Person::EmploymentCase::kSelfEmployed:
                    sum += 'self';
                    break;
                default:
                    break;
                }
            }
        }
    }
    const auto end = std::chrono::steady_clock::now();
    // Optional:  Delete all global objects allocated by libprotobuf.
    //google::protobuf::ShutdownProtobufLibrary();
    //std::cout << "protobuf check: " << sum << std::endl;
    return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())
        / static_cast<float>(reps);
}
