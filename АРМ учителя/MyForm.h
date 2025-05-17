#pragma once
#include "Topic.h"
#include "StudentsManager.h"
#using <Microsoft.VisualBasic.dll>
using namespace Microsoft::VisualBasic;

namespace формс {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;
    using namespace System::IO;
    using namespace System::Xml::Serialization;

    public ref class MyForm : public Form {
    public:
        MyForm() {
            InitializeComponent();
            topics = gcnew List<Topic^>();
            allStudents = gcnew List<String^>();
            autoCompleteStudents = gcnew AutoCompleteStringCollection();

            this->BackColor = Color::FromArgb(240, 245, 249);
            this->Font = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Regular);

            LoadAllData();
            RefreshTable();
            RefreshStudentsList();
        }

    protected:
        ~MyForm() {
            if (components) delete components;
        }

    private:
        List<Topic^>^ topics;
        List<String^>^ allStudents;
        AutoCompleteStringCollection^ autoCompleteStudents;
        ListView^ topicTable;
        ListView^ studentsList;
        TextBox^ nameBox;
        TextBox^ newStudentBox;
        System::ComponentModel::Container^ components;
        Button^ CreateButton(String^ text, Point location, Drawing::Size size,
            EventHandler^ handler, Control^ parent, Color backColor) {
            Button^ btn = gcnew Button();
            btn->Text = text;
            btn->Location = location;
            btn->Size = size;
            btn->Font = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);
            btn->BackColor = backColor;
            btn->ForeColor = Color::White;
            btn->FlatStyle = FlatStyle::Flat;
            btn->FlatAppearance->BorderSize = 0;
            btn->Cursor = Cursors::Hand;
            btn->Click += handler;
            parent->Controls->Add(btn);
            return btn;
        }

    private:
        void InitializeComponent(void) {
            this->SuspendLayout();
            this->Text = L"Учительский Помощник";
            this->Size = System::Drawing::Size(1000, 750);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->FormClosing += gcnew FormClosingEventHandler(this, &MyForm::FormClosingHandler);

            Panel^ headerPanel = gcnew Panel();
            headerPanel->BackColor = Color::FromArgb(63, 81, 181);
            headerPanel->Dock = DockStyle::Top;
            headerPanel->Height = 60;

            Label^ titleLabel = gcnew Label();
            titleLabel->Text = L"УЧИТЕЛЬСКИЙ ПОМОЩНИК";
            titleLabel->Font = gcnew System::Drawing::Font("Segoe UI", 14, FontStyle::Bold);
            titleLabel->ForeColor = Color::White;
            titleLabel->Dock = DockStyle::Fill;
            titleLabel->TextAlign = ContentAlignment::MiddleCenter;
            headerPanel->Controls->Add(titleLabel);
            this->Controls->Add(headerPanel);

            topicTable = gcnew ListView();
            topicTable->View = View::Details;
            topicTable->FullRowSelect = true;
            topicTable->GridLines = false;
            topicTable->BorderStyle = BorderStyle::None;
            topicTable->Location = Point(30, 90);
            topicTable->Size = Drawing::Size(600, 300);
            topicTable->Font = gcnew System::Drawing::Font("Segoe UI", 9);
            topicTable->BackColor = Color::White;
            topicTable->OwnerDraw = true;
            topicTable->DrawColumnHeader += gcnew DrawListViewColumnHeaderEventHandler(this, &MyForm::DrawColumnHeader);
            topicTable->DrawItem += gcnew DrawListViewItemEventHandler(this, &MyForm::DrawListViewItem);
            topicTable->DrawSubItem += gcnew DrawListViewSubItemEventHandler(this, &MyForm::DrawSubItem);

            topicTable->Columns->Add("№", 50, HorizontalAlignment::Center);
            topicTable->Columns->Add("Тема", 200);
            topicTable->Columns->Add("Тестов", 80, HorizontalAlignment::Center);
            topicTable->Columns->Add("Объяснений", 100, HorizontalAlignment::Center);
            topicTable->Columns->Add("Ученики", 90, HorizontalAlignment::Center);
            topicTable->Columns->Add("Сред. понимание", 120, HorizontalAlignment::Center);
            this->Controls->Add(topicTable);

            // Список учеников
            GroupBox^ studentsGroup = gcnew GroupBox();
            studentsGroup->Text = L"Список учеников";
            studentsGroup->Location = Point(650, 90);
            studentsGroup->Size = Drawing::Size(320, 300);
            studentsGroup->Font = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);

            studentsList = gcnew ListView();
            studentsList->View = View::Details;
            studentsList->FullRowSelect = true;
            studentsList->GridLines = false;
            studentsList->BorderStyle = BorderStyle::None;
            studentsList->Location = Point(10, 20);
            studentsList->Size = Drawing::Size(300, 200);
            studentsList->Font = gcnew System::Drawing::Font("Segoe UI", 9);
            studentsList->BackColor = Color::White;
            studentsList->Columns->Add("Имя ученика", 280);
            studentsList->MultiSelect = false;
            studentsGroup->Controls->Add(studentsList);

            newStudentBox = gcnew TextBox();
            newStudentBox->Location = Point(10, 230);
            newStudentBox->Size = Drawing::Size(200, 25);
            newStudentBox->Font = gcnew System::Drawing::Font("Segoe UI", 9);
            newStudentBox->BorderStyle = BorderStyle::FixedSingle;
            studentsGroup->Controls->Add(newStudentBox);

            CreateButton(L"Добавить", Point(220, 230), Drawing::Size(90, 25),
                gcnew EventHandler(this, &MyForm::AddNewStudent), studentsGroup, Color::FromArgb(76, 175, 80));
            CreateButton(L"Удалить", Point(220, 260), Drawing::Size(90, 25),
                gcnew EventHandler(this, &MyForm::RemoveStudent), studentsGroup, Color::FromArgb(244, 67, 54));
            this->Controls->Add(studentsGroup);

           
            GroupBox^ addGroup = gcnew GroupBox();
            addGroup->Text = L"Добавить новую тему";
            addGroup->Location = Point(30, 400);
            addGroup->Size = Drawing::Size(600, 70);
            addGroup->Font = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);

            nameBox = gcnew TextBox();
            nameBox->Location = Point(20, 30);
            nameBox->Size = Drawing::Size(300, 25);
            nameBox->Font = gcnew System::Drawing::Font("Segoe UI", 9);
            nameBox->BorderStyle = BorderStyle::FixedSingle;
            addGroup->Controls->Add(nameBox);

            CreateButton(L"Добавить тему", Point(340, 28), Drawing::Size(140, 30),
                gcnew EventHandler(this, &MyForm::AddTopic), addGroup, Color::FromArgb(33, 150, 243));
            CreateButton(L"+ Тест", Point(490, 28), Drawing::Size(100, 30),
                gcnew EventHandler(this, &MyForm::AddTest), addGroup, Color::FromArgb(33, 150, 243));
            this->Controls->Add(addGroup);

           
            GroupBox^ actionsGroup = gcnew GroupBox();
            actionsGroup->Text = L"Действия с выбранной темой";
            actionsGroup->Location = Point(30, 480);
            actionsGroup->Size = Drawing::Size(600, 120);
            actionsGroup->Font = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);

            CreateButton(L"+ Объяснение", Point(20, 30), Drawing::Size(180, 30),
                gcnew EventHandler(this, &MyForm::AddExplanation), actionsGroup, Color::FromArgb(255, 152, 0));
            CreateButton(L"+ Оценка", Point(210, 30), Drawing::Size(180, 30),
                gcnew EventHandler(this, &MyForm::AddStudent), actionsGroup, Color::FromArgb(255, 152, 0));
            CreateButton(L"Просмотр оценок", Point(400, 30), Drawing::Size(180, 30),
                gcnew EventHandler(this, &MyForm::ViewScores), actionsGroup, Color::FromArgb(255, 152, 0));
            CreateButton(L"Удалить тему", Point(20, 70), Drawing::Size(180, 30),
                gcnew EventHandler(this, &MyForm::DeleteTopic), actionsGroup, Color::FromArgb(255, 152, 0));
            CreateButton(L"Даты объяснений", Point(210, 70), Drawing::Size(180, 30),
                gcnew EventHandler(this, &MyForm::ViewExplanationDates), actionsGroup, Color::FromArgb(255, 152, 0));
            CreateButton(L"Даты тестов", Point(400, 70), Drawing::Size(180, 30),
                gcnew EventHandler(this, &MyForm::ViewTestDates), actionsGroup, Color::FromArgb(255, 152, 0));
            this->Controls->Add(actionsGroup);

            this->ResumeLayout(false);
        }

        void RefreshStudentsList() {
            studentsList->Items->Clear();
            allStudents->Sort();
            for each(String ^ student in allStudents) {
                studentsList->Items->Add(gcnew ListViewItem(student));
            }
        }

        void AddNewStudent(Object^ sender, EventArgs^ e) {
            String^ name = newStudentBox->Text->Trim();
            if (!String::IsNullOrWhiteSpace(name) && !allStudents->Contains(name)) {
                allStudents->Add(name);
                allStudents->Sort();
                autoCompleteStudents->Add(name);
                newStudentBox->Clear();
                RefreshStudentsList();
                SaveAllData();
            }
        }

        void RemoveStudent(Object^ sender, EventArgs^ e) {
            if (studentsList->SelectedItems->Count > 0) {
                String^ name = studentsList->SelectedItems[0]->Text;
                allStudents->Remove(name);
                autoCompleteStudents->Remove(name);

                for each(Topic ^ topic in topics) {
                    if (topic->StudentNames->Contains(name)) {
                        topic->StudentNames->Remove(name);
                        topic->StudentScores->Remove(name);
                    }
                }

                RefreshStudentsList();
                RefreshTable();
                SaveAllData();
            }
        }

        void DrawColumnHeader(Object^ sender, DrawListViewColumnHeaderEventArgs^ e) {
            e->DrawBackground();
            e->Graphics->FillRectangle(gcnew SolidBrush(Color::FromArgb(63, 81, 181)), e->Bounds);
            TextFormatFlags flags = TextFormatFlags::Left | TextFormatFlags::VerticalCenter;
            TextRenderer::DrawText(e->Graphics, e->Header->Text, gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold),
                e->Bounds, Color::White, flags);
        }

        void DrawListViewItem(Object^ sender, DrawListViewItemEventArgs^ e) {
            e->DrawDefault = true;
        }

        void DrawSubItem(Object^ sender, DrawListViewSubItemEventArgs^ e) {
            if (e->Item->Selected) {
                e->Graphics->FillRectangle(gcnew SolidBrush(Color::FromArgb(207, 216, 220)), e->Bounds);
            }
            else {
                e->Graphics->FillRectangle(gcnew SolidBrush(Color::White), e->Bounds);
            }

            // Обработка подсветки низких оценок
            if (e->ColumnIndex == 5) { // Колонка "Сред. понимание"
                double avgScore;
                if (Double::TryParse(e->SubItem->Text, avgScore)) {
                    if (avgScore < 3.0) {
                        e->Graphics->FillRectangle(gcnew SolidBrush(Color::FromArgb(255, 235, 238)), e->Bounds);
                    }
                }
            }

            TextFormatFlags flags = TextFormatFlags::Left | TextFormatFlags::VerticalCenter;
            if (e->ColumnIndex == 0 || e->ColumnIndex == 2 || e->ColumnIndex == 3 || e->ColumnIndex == 4 || e->ColumnIndex == 5) {
                flags = TextFormatFlags::HorizontalCenter | TextFormatFlags::VerticalCenter;
            }

            TextRenderer::DrawText(e->Graphics, e->SubItem->Text, e->Item->Font, e->Bounds, Color::FromArgb(33, 33, 33), flags);
        }


        void LoadAllData() {
            try {
                if (File::Exists("topics.xml")) {
                    XmlSerializer^ serializer = gcnew XmlSerializer(List<Topic^>::typeid);
                    StreamReader^ reader = gcnew StreamReader("topics.xml");
                    topics = (List<Topic^>^)serializer->Deserialize(reader);
                    reader->Close();

                    for each (Topic ^ t in topics) {
                        t->RebuildStudentScoresFromSerializable();
                    }
                }

                allStudents = StudentsManager::LoadStudents();
                autoCompleteStudents->Clear();
                for each (String ^ student in allStudents) {
                    autoCompleteStudents->Add(student);
                }
            }
            catch (Exception^ e) {
                MessageBox::Show("Ошибка загрузки данных: " + e->Message, "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                topics = gcnew List<Topic^>();
                allStudents = gcnew List<String^>();
            }
        }

        void SaveAllData() {
            try {
                for each (Topic ^ t in topics) {
                    t->PrepareStudentScoresForSerialization();
                }

                XmlSerializer^ serializer = gcnew XmlSerializer(List<Topic^>::typeid);
                StreamWriter^ writer = gcnew StreamWriter("topics.xml");
                serializer->Serialize(writer, topics);
                writer->Close();

                StudentsManager::SaveStudents(allStudents);
            }
            catch (Exception^ e) {
                MessageBox::Show("Ошибка сохранения данных: " + e->Message, "Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void FormClosingHandler(Object^ sender, FormClosingEventArgs^ e) {
            SaveAllData();
        }

        void RefreshTable() {
            topicTable->Items->Clear();
            for (int i = 0; i < topics->Count; ++i) {
                Topic^ t = topics[i];
                ListViewItem^ item = gcnew ListViewItem((i + 1).ToString());
                item->SubItems->Add(t->Name);
                item->SubItems->Add(t->Tests.ToString());
                item->SubItems->Add(t->Explanations.ToString());
                item->SubItems->Add(t->StudentNames->Count.ToString());
                item->SubItems->Add(t->GetAverageUnderstanding().ToString("F2"));

                if (t->GetAverageUnderstanding() < 3.0)
                    item->BackColor = Color::LightPink;

                topicTable->Items->Add(item);
            }
        }

        int GetSelectedIndex() {
            return topicTable->SelectedIndices->Count > 0 ? topicTable->SelectedIndices[0] : -1;
        }

        void AddTopic(Object^ sender, EventArgs^ e) {
            String^ name = nameBox->Text->Trim();
            if (!String::IsNullOrWhiteSpace(name)) {
                Topic^ t = gcnew Topic();
                t->Name = name;
                topics->Add(t);
                nameBox->Clear();
                RefreshTable();
                SaveAllData();
            }
        }

        void AddTest(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                topics[idx]->AddTest();
                RefreshTable();
                SaveAllData();
            }
        }

        void AddExplanation(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                topics[idx]->Explanations++;
                topics[idx]->ExplanationDates->Add(DateTime::Now);
                RefreshTable();
                SaveAllData();
            }
        }

        void AddStudent(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                Form^ inputForm = gcnew Form();
                inputForm->Text = L"Добавление оценки";
                inputForm->Size = Drawing::Size(300, 180);

                TextBox^ nameBox = gcnew TextBox();
                nameBox->Location = Point(120, 10);
                nameBox->Width = 150;
                nameBox->AutoCompleteSource = AutoCompleteSource::CustomSource;
                nameBox->AutoCompleteMode = AutoCompleteMode::SuggestAppend;
                nameBox->AutoCompleteCustomSource = autoCompleteStudents;

                TextBox^ scoreBox = gcnew TextBox();
                scoreBox->Location = Point(120, 40);
                scoreBox->Width = 50;

                Label^ nameLabel = gcnew Label();
                nameLabel->Text = "Имя ученика:";
                nameLabel->Location = Point(10, 10);
                inputForm->Controls->Add(nameLabel);

                inputForm->Controls->Add(nameBox);
                Label^ scoreLabel = gcnew Label();
                scoreLabel->Text = "Оценка (0-5):";
                scoreLabel->Location = Point(10, 40);
                inputForm->Controls->Add(scoreLabel);

                inputForm->Controls->Add(scoreBox);

                Button^ okButton = gcnew Button();
                okButton->Text = L"OK";
                okButton->DialogResult = System::Windows::Forms::DialogResult::OK;
                okButton->Location = Point(100, 80);
                inputForm->AcceptButton = okButton;
                inputForm->Controls->Add(okButton);

                if (inputForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                    String^ name = nameBox->Text->Trim();
                    int score;
                    if (!Int32::TryParse(scoreBox->Text, score) || score < 0 || score > 5) {
                        MessageBox::Show(L"Некорректная оценка.");
                        return;
                    }

                    topics[idx]->AddStudentScore(name, score);

                    if (!allStudents->Contains(name)) {
                        allStudents->Add(name);
                        autoCompleteStudents->Add(name);
                    }

                    RefreshTable();
                    SaveAllData();
                }
            }
        }

        void ViewScores(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                Form^ scoresForm = gcnew Form();
                scoresForm->Text = "Оценки: " + topics[idx]->Name;
                scoresForm->Size = Drawing::Size(600, 400);

                DataGridView^ grid = gcnew DataGridView();
                grid->Dock = DockStyle::Fill;
                grid->ReadOnly = true;

                grid->Columns->Add("Student", "Ученик");
                for (int i = 1; i <= topics[idx]->Tests; i++) {
                    grid->Columns->Add("Test" + i, "Тест " + i);
                }
                grid->Columns->Add("Average", "Среднее");
                topics[idx]->StudentNames->Sort();
                for each(String ^ name in topics[idx]->StudentNames) {
                    array<Object^>^ row = gcnew array<Object^>(topics[idx]->Tests + 2);
                    row[0] = name;

                    double sum = 0;
                    int count = 0;

                    if (topics[idx]->StudentScores->ContainsKey(name)) {
                        array<int>^ scores = topics[idx]->StudentScores[name];
                        for (int i = 0; i < topics[idx]->Tests; i++) {
                            int score = (i < scores->Length) ? scores[i] : 0;
                            row[i + 1] = score > 0 ? score.ToString() : "-";
                            if (score > 0) {
                                sum += score;
                                count++;
                            }
                        }
                    }

                    row[topics[idx]->Tests + 1] = count > 0 ? (sum / count).ToString("F1") : "-";
                    grid->Rows->Add(row);
                }

                scoresForm->Controls->Add(grid);
                scoresForm->ShowDialog();
            }
        }

        void ViewExplanationDates(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                MessageBox::Show(topics[idx]->GetExplanationHistory(),
                    "Даты объяснений: " + topics[idx]->Name);
            }
        }

        void ViewTestDates(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                MessageBox::Show(topics[idx]->GetTestHistory(),
                    "Даты тестов: " + topics[idx]->Name);
            }
        }

        void DeleteTopic(Object^ sender, EventArgs^ e) {
            int idx = GetSelectedIndex();
            if (idx >= 0) {
                topics->RemoveAt(idx);
                RefreshTable();
                SaveAllData();
            }
        }

        void ManageStudentsClick(Object^ sender, EventArgs^ e) {
            Form^ studentsForm = gcnew Form();
            studentsForm->Text = L"Управление учениками";
            studentsForm->Size = Drawing::Size(400, 300);

            ListBox^ listBox = gcnew ListBox();
            listBox->Size = Drawing::Size(350, 150);
            listBox->Location = Point(20, 20);
            studentsForm->Controls->Add(listBox);

            TextBox^ nameBox = gcnew TextBox();
            nameBox->Size = Drawing::Size(350, 20);
            nameBox->Location = Point(20, 180);
            nameBox->AutoCompleteSource = AutoCompleteSource::CustomSource;
            nameBox->AutoCompleteMode = AutoCompleteMode::SuggestAppend;
            nameBox->AutoCompleteCustomSource = autoCompleteStudents;
            studentsForm->Controls->Add(nameBox);

            Button^ addBtn = gcnew Button();
            addBtn->Text = L"Добавить";
            addBtn->Location = Point(20, 210);
            addBtn->Click += gcnew EventHandler(this, &MyForm::AddStudentHandler);
            studentsForm->Controls->Add(addBtn);

            Button^ removeBtn = gcnew Button();
            removeBtn->Text = L"Удалить";
            removeBtn->Location = Point(120, 210);
            removeBtn->Click += gcnew EventHandler(this, &MyForm::RemoveStudentHandler);
            studentsForm->Controls->Add(removeBtn);

            // Заполнение списка
            for each (String ^ student in allStudents) {
                listBox->Items->Add(student);
            }

            studentsForm->ShowDialog();
        }

        void AddStudentHandler(Object^ sender, EventArgs^ e) {
            Button^ btn = (Button^)sender;
            Form^ form = (Form^)btn->Parent;
            TextBox^ nameBox = (TextBox^)form->Controls[1];
            ListBox^ listBox = (ListBox^)form->Controls[0];

            String^ name = nameBox->Text->Trim();
            if (!String::IsNullOrWhiteSpace(name) && !allStudents->Contains(name)) {
                allStudents->Add(name);
                listBox->Items->Add(name);
                autoCompleteStudents->Add(name);
                nameBox->Clear();
                SaveAllData();
            }
        }

        void RemoveStudentHandler(Object^ sender, EventArgs^ e) {
            Button^ btn = (Button^)sender;
            Form^ form = (Form^)btn->Parent;
            ListBox^ listBox = (ListBox^)form->Controls[0];

            if (listBox->SelectedIndex >= 0) {
                String^ name = listBox->SelectedItem->ToString();
                allStudents->Remove(name);
                autoCompleteStudents->Remove(name);
                listBox->Items->RemoveAt(listBox->SelectedIndex);

                // Удаляем ученика из всех тем
                for each (Topic ^ topic in topics) {
                    if (topic->StudentNames->Contains(name)) {
                        topic->StudentNames->Remove(name);
                        topic->StudentScores->Remove(name);
                    }
                }

                SaveAllData();
            }
        }
    };
}