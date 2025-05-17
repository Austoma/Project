#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Xml::Serialization;

public ref class StudentsManager abstract sealed {
public:
    static List<String^>^ LoadStudents() {
        try {
            if (File::Exists("students.xml")) {
                XmlSerializer^ serializer = gcnew XmlSerializer(List<String^>::typeid);
                StreamReader^ reader = gcnew StreamReader("students.xml");
                List<String^>^ students = (List<String^>^)serializer->Deserialize(reader);
                reader->Close();
                return students != nullptr ? students : gcnew List<String^>();
            }
            return gcnew List<String^>();
        }
        catch (Exception^ e) {
            throw gcnew Exception("Ќе удалось загрузить список учеников: " + e->Message);
        }
    }

    static void SaveStudents(List<String^>^ students) {
        try {
            if (students == nullptr) {
                students = gcnew List<String^>();
            }

            XmlSerializer^ serializer = gcnew XmlSerializer(List<String^>::typeid);
            StreamWriter^ writer = gcnew StreamWriter("students.xml");
            serializer->Serialize(writer, students);
            writer->Close();
        }
        catch (Exception^ e) {
            throw gcnew Exception("Ќе удалось сохранить список учеников: " + e->Message);
        }
    }
};