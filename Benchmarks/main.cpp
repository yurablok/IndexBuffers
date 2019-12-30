#include <iostream>
#include <chrono>

//> protobuf\protoc-3.11.2\bin\protoc.exe --cpp_out=. protobuf\addressbook.proto
#include "protobuf/addressbook.pb.h"
#ifdef _WIN32
#   ifdef _DEBUG
#       pragma comment(lib, "libprotobufd.lib")
#   else
#       pragma comment(lib, "libprotobuf.lib")
#   endif
#endif
std::chrono::milliseconds run_protobuf(const uint32_t reps) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    volatile int32_t sum = 0;
    uint8_t buffer[512];
    const auto begin = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < reps; ++i) {
        proto::AddressBook addressBookSrc;
        {
            auto alice = addressBookSrc.add_people();
            alice->set_id(123);
            alice->set_name("Alice");
            alice->set_email("alice@example.com");
            auto alicePhones = alice->add_phones();
            alicePhones->set_number("555-1212");
            alicePhones->set_type(proto::Person_PhoneType_MOBILE);
            //alice->employment school MIT
            alice->set_school("MIT");

            auto bob = addressBookSrc.add_people();
            bob->set_id(456);
            bob->set_name("Bob");
            bob->set_email("bob@example.com");
            auto bobPhones = bob->add_phones();
            bobPhones->set_number("555-4567");
            bobPhones->set_type(proto::Person_PhoneType_HOME);
            bobPhones = bob->add_phones();
            bobPhones->set_number("555-7654");
            bobPhones->set_type(proto::Person_PhoneType_WORK);
            //bob->employment unemployed
            bob->set_unemployed(true);
            addressBookSrc.SerializeToArray(buffer, sizeof(buffer));
        }
        const uint32_t size = addressBookSrc.ByteSizeLong(); // 106
        proto::AddressBook addressBookDst;
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
                case proto::Person::EmploymentCase::kUnemployed:
                    sum += 'unem';
                    break;
                case proto::Person::EmploymentCase::kEmployer:
                    sum += 'empl';
                    sum += person.employer().size();
                    break;
                case proto::Person::EmploymentCase::kSchool:
                    sum += 'scho';
                    sum += person.school().size();
                    break;
                case proto::Person::EmploymentCase::kSelfEmployed:
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
    google::protobuf::ShutdownProtobufLibrary();
    //std::cout << "protobuf check: " << sum << std::endl;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
}


//> flatbuffers\flatc-1.11.0\flatc.exe -o flatbuffers --scoped-enums --cpp flatbuffers\addressbook.fbs
#include "flatbuffers/addressbook_generated.h"
std::chrono::milliseconds run_flatbuffers(const uint32_t reps) {
    volatile int32_t sum = 0;
    flatbuffers::FlatBufferBuilder builder;
    const auto begin = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < reps; ++i) {
        builder.Clear();
        {
            std::vector<flatbuffers::Offset<fbs::Person>> people;

            auto aliceEmployment = fbs::CreateschoolDirect(builder,
                "MIT");
            std::vector<flatbuffers::Offset<fbs::PhoneNumber>> alicePhones;
            alicePhones.push_back(fbs::CreatePhoneNumberDirect(builder,
                "555-1212",
                fbs::PhoneNumberType::mobile));
            people.push_back(fbs::CreatePersonDirect(builder,
                123,
                "Alice",
                "alice@example.com",
                &alicePhones,
                fbs::Employment::school,
                aliceEmployment.o));

            auto bobEmployment = fbs::Createunemployed(builder);
            std::vector<flatbuffers::Offset<fbs::PhoneNumber>> bobPhones;
            bobPhones.push_back(fbs::CreatePhoneNumberDirect(builder,
                "555-4567",
                fbs::PhoneNumberType::home));
            bobPhones.push_back(fbs::CreatePhoneNumberDirect(builder,
                "555-7654",
                fbs::PhoneNumberType::work));
            people.push_back(fbs::CreatePersonDirect(builder,
                456,
                "Bob",
                "bob@example.com",
                &bobPhones,
                fbs::Employment::unemployed,
                bobEmployment.o));

            builder.Finish(fbs::CreateAddressBookDirect(
                builder, &people));
        }
        const uint32_t size = builder.GetSize(); // 296
        {
            auto addressBook = fbs::GetAddressBook(builder.GetBufferPointer());
            for (uint8_t peopleIt = 0, peopleSize = addressBook->people()->size();
                    peopleIt < peopleSize; ++peopleIt) {
                auto person = addressBook->people()->Get(peopleIt);
                sum += person->name()->size();
                for (uint8_t phoneIt = 0, phoneSize = person->phones()->size();
                        phoneIt < phoneSize; ++phoneIt) {
                    auto phone = person->phones()->Get(phoneIt);
                    sum += static_cast<int32_t>(phone->type());
                    sum += phone->number()->size();
                }
                switch (person->employment_type()) {
                case fbs::Employment::unemployed:
                    sum += 'unem';
                    break;
                case fbs::Employment::employer:
                    sum += 'empl';
                    sum += person->employment_as_employer()->text()->size();
                    break;
                case fbs::Employment::school:
                    sum += 'scho';
                    sum += person->employment_as_school()->text()->size();
                    break;
                case fbs::Employment::selfEmployed:
                    sum += 'self';
                    break;
                default:
                    break;
                }
            }
        }
    }
    const auto end = std::chrono::steady_clock::now();
    //std::cout << "flatbuffers check: " << sum << std::endl;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
}


//> capnp compile -oc++ addressbook.capnp
#include "capnproto/addressbook.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#ifdef _WIN32
#   ifdef _DEBUG
#       pragma comment(lib, "capnpd.lib")
#       pragma comment(lib, "kjd.lib")
#   else
#       pragma comment(lib, "capnp.lib")
#       pragma comment(lib, "kj.lib")
#   endif
#endif
std::chrono::milliseconds run_capnproto(const uint32_t reps) {
    volatile int32_t sum = 0;
    const auto begin = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < reps; ++i) {
        kj::VectorOutputStream output;
        ::capnp::MallocMessageBuilder messageSrc;
        {
            AddressBook::Builder addressBook = messageSrc.initRoot<AddressBook>();
            ::capnp::List<Person>::Builder people = addressBook.initPeople(2);

            Person::Builder alice = people[0];
            alice.setId(123);
            alice.setName("Alice");
            alice.setEmail("alice@example.com");
            auto alicePhones = alice.initPhones(1);
            alicePhones[0].setNumber("555-1212");
            alicePhones[0].setType(Person::PhoneNumber::Type::MOBILE);
            alice.getEmployment().setSchool("MIT");

            Person::Builder bob = people[1];
            bob.setId(456);
            bob.setName("Bob");
            bob.setEmail("bob@example.com");
            auto bobPhones = bob.initPhones(2);
            bobPhones[0].setNumber("555-4567");
            bobPhones[0].setType(Person::PhoneNumber::Type::HOME);
            bobPhones[1].setNumber("555-7654");
            bobPhones[1].setType(Person::PhoneNumber::Type::WORK);
            bob.getEmployment().setUnemployed();

            writePackedMessage(output, messageSrc);
        }
        const uint32_t size = output.getArray().size(); // 151
        kj::ArrayInputStream input(output.getArray());
        ::capnp::PackedMessageReader messageDst(input);
        {
            AddressBook::Reader addressBook = messageDst.getRoot<AddressBook>();

            for (Person::Reader person : addressBook.getPeople()) {
                sum += person.getName().size();
                for (Person::PhoneNumber::Reader phone : person.getPhones()) {
                    sum += static_cast<int32_t>(phone.getType());
                    sum += phone.getNumber().size();
                }
                Person::Employment::Reader employment = person.getEmployment();
                switch (employment.which()) {
                case Person::Employment::UNEMPLOYED:
                    sum += 'unem';
                    break;
                case Person::Employment::EMPLOYER:
                    sum += 'empl';
                    sum += employment.getEmployer().size();
                    break;
                case Person::Employment::SCHOOL:
                    sum += 'scho';
                    sum += employment.getSchool().size();
                    break;
                case Person::Employment::SELF_EMPLOYED:
                    sum += 'self';
                    break;
                default:
                    break;
                }
            }
        }
    }
    const auto end = std::chrono::steady_clock::now();
    //std::cout << "capnproto check: " << sum << std::endl;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
}


//> ..\x64\Release\ibc_x64.exe -i IndexBuffers\addressbook.ibs
#include "IndexBuffers/addressbook_generated.hpp"
std::chrono::milliseconds run_IndexBuffers(const uint32_t reps) {
    volatile int32_t sum = 0;
    auto buffer = std::make_shared<std::vector<uint8_t>>();
    const auto begin = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < reps; ++i) {
        ibs::AddressBook addressBookSrc;
        {
            addressBookSrc.create(buffer);
            addressBookSrc.set_people(2);

            auto& alice = addressBookSrc.get_people(0);
            alice.set_id(123);
            alice.set_name<std::string>("Alice");
            alice.set_email<std::string>("alice@example.com");
            alice.set_phones(1);
            auto& alicePhone0 = alice.get_phones(0);
            alicePhone0.set_number<std::string>("555-1212");
            alicePhone0.set_type(ibs::PhoneNumberType::mobile);

            alice.set_employment();
            alice.get_employment().set_school<std::string>("MIT");

            auto& bob = addressBookSrc.get_people(1);
            bob.set_id(456);
            bob.set_name<std::string>("Bob");
            bob.set_email<std::string>("bob@example.com");
            bob.set_phones(2);
            auto& bobPhone0 = bob.get_phones(0);
            bobPhone0.set_number<std::string>("555-4567");
            bobPhone0.set_type(ibs::PhoneNumberType::home);
            auto& bobPhone1 = bob.get_phones(1);
            bobPhone1.set_number<std::string>("555-7654");
            bobPhone1.set_type(ibs::PhoneNumberType::work);

            bob.set_employment();
            bob.get_employment().set_unemployed(0);
        }
        // if offset type is:
        //  uint8 - 91
        //  uint16 - 114
        const uint32_t size = addressBookSrc.size();
        ibs::AddressBook addressBookDst;
        {
            addressBookDst.from(addressBookSrc.to(), addressBookSrc.size());
            for (uint8_t personIt = 0, personSize = addressBookDst.size_people();
                    personIt < personSize; ++personIt) {
                auto& person = addressBookDst.get_people(personIt);
                sum += person.size_name();
                for (uint8_t phoneIt = 0, phoneSize = person.size_phones();
                        phoneIt < phoneSize; ++phoneIt) {
                    auto& phone = person.get_phones(phoneIt);
                    sum += phone.get_type();
                    sum += phone.size_number();
                }
                auto& employment = person.get_employment();
                switch (employment.variant()) {
                case Person::Employment::UNEMPLOYED:
                    sum += 'unem';
                    break;
                case Person::Employment::EMPLOYER:
                    sum += 'empl';
                    sum += employment.size_employer();
                    break;
                case Person::Employment::SCHOOL:
                    sum += 'scho';
                    sum += employment.size_school();
                    break;
                case Person::Employment::SELF_EMPLOYED:
                    sum += 'self';
                    break;
                default:
                    break;
                }
            }
        }
    }
    const auto end = std::chrono::steady_clock::now();
    //std::cout << "IndexBuffers check: " << sum << std::endl;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
}


int main() {
    system("pause");
    const uint32_t reps = 5000000;
    //const uint32_t reps = 1;
    std::cout << "protobuf:     " << run_protobuf(reps).count() << " ms" << std::endl;
    std::cout << "flatbuffers:  " << run_flatbuffers(reps).count() << " ms" << std::endl;
    std::cout << "capnproto:    " << run_capnproto(reps).count() << " ms" << std::endl;
    std::cout << "IndexBuffers: " << run_IndexBuffers(reps).count() << " ms" << std::endl;
    system("pause");
}
