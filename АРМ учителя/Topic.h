#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Xml::Serialization;
using namespace System::Windows::Forms;

[Serializable]
public ref class StudentScoreEntry {
public:
    String^ Name;
    array<int>^ Scores;
};

[Serializable]
[XmlRoot("Topic")]
public ref class Topic {
public:
    [XmlElement("Name")]
    String^ Name;

    [XmlElement("Explanations")]
    int Explanations;

    [XmlElement("Tests")]
    int Tests;

    [XmlIgnore]
    Dictionary<String^, array<int>^>^ StudentScores;

    [XmlArray("StudentNames")]
    [XmlArrayItem("StudentName")]
    List<String^>^ StudentNames;

    [XmlArray("TestDates")]
    [XmlArrayItem("TestDate")]
    List<DateTime>^ TestDates;

    [XmlArray("ExplanationDates")]
    [XmlArrayItem("ExplanationDate")]
    List<DateTime>^ ExplanationDates;

    [XmlArray("StudentScores")]
    [XmlArrayItem("Entry")]
    List<StudentScoreEntry^>^ StudentScoresSerializable;

    Topic() {
        Name = "";
        Explanations = 0;
        Tests = 0;
        StudentScores = gcnew Dictionary<String^, array<int>^>();
        StudentNames = gcnew List<String^>();
        TestDates = gcnew List<DateTime>();
        ExplanationDates = gcnew List<DateTime>();
        StudentScoresSerializable = gcnew List<StudentScoreEntry^>();
    }

    void PrepareStudentScoresForSerialization() {
        StudentScoresSerializable = gcnew List<StudentScoreEntry^>();

        for each (KeyValuePair<String^, array<int>^> kvp in StudentScores) {
            StudentScoreEntry^ entry = gcnew StudentScoreEntry();
            entry->Name = kvp.Key;
            entry->Scores = kvp.Value;
            StudentScoresSerializable->Add(entry);
        }
    }

    void RebuildStudentScoresFromSerializable() {
        StudentScores = gcnew Dictionary<String^, array<int>^>();

        if (StudentScoresSerializable == nullptr)
            return;

        for each (StudentScoreEntry ^ entry in StudentScoresSerializable) {
            if (entry->Name != nullptr && entry->Scores != nullptr) {
                StudentScores[entry->Name] = entry->Scores;
            }
        }
    }

    void AddStudentScore(String^ studentName, int score) {
        if (!StudentNames->Contains(studentName)) {
            StudentNames->Add(studentName);
            // Сортируем список после добавления
            StudentNames->Sort();
        }

        array<int>^ scores;
        if (!StudentScores->TryGetValue(studentName, scores)) {
            scores = gcnew array<int>(Tests);
            StudentScores[studentName] = scores;
        }

        if (Tests > 0) {
            scores[Tests - 1] = score;
            StudentScores[studentName] = scores;
        }
    }

    String^ GetStudentScores() {
        String^ result = "";
        for each (String ^ student in StudentNames) {
            array<int>^ scores;
            if (StudentScores->TryGetValue(student, scores)) {
                result += student + ": ";
                for each (int score in scores) {
                    result += score.ToString() + " ";
                }
                result += "\n";
            }
        }
        return result;
    }

    double GetAverageUnderstanding() {
        if (StudentScores->Count == 0) return 0.0;

        double total = 0.0;
        int count = 0;

        for each (KeyValuePair<String^, array<int>^> kvp in StudentScores) {
            for each (int score in kvp.Value) {
                if (score > 0) {
                    total += score;
                    count++;
                }
            }
        }

        return count > 0 ? total / count : 0.0;
    }

    void AddTest() {
        Tests++;
        TestDates->Add(DateTime::Now);

        for each (String ^ student in StudentNames) {
            array<int>^ oldScores;
            array<int>^ newScores = gcnew array<int>(Tests);

            if (StudentScores->TryGetValue(student, oldScores)) {
                Array::Copy(oldScores, newScores, oldScores->Length);
            }

            StudentScores[student] = newScores;
        }
    }

    String^ GetScoresTable() {
        String^ table = "Ученик\\Тест\t";
        for (int i = 1; i <= Tests; i++) {
            table += i.ToString() + "\t";
        }
        table += "Среднее\n";

        StudentNames->Sort();

        for each (String ^ name in StudentNames) {
            table += name + "\t";
            array<int>^ scores;
            double sum = 0;
            int count = 0;

            if (StudentScores->TryGetValue(name, scores)) {
                for (int i = 0; i < Tests; i++) {
                    int score = scores[i];
                    table += (score > 0 ? score.ToString() : "-") + "\t";
                    if (score > 0) {
                        sum += score;
                        count++;
                    }
                }
            }

            table += (count > 0 ? (sum / count).ToString("F1") : "-") + "\n";
        }

        return table;
    }

    String^ GetExplanationHistory() {
        String^ history = "Даты объяснений:\n";
        for each (DateTime date in ExplanationDates) {
            history += date.ToString("dd.MM.yyyy") + "\n";
        }
        return history;
    }

    String^ GetTestHistory() {
        String^ history = "Даты тестов:\n";
        for each (DateTime date in TestDates) {
            history += date.ToString("dd.MM.yyyy") + "\n";
        }
        return history;
    }
};
