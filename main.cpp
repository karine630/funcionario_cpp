#include <iostream>
#include <string>
#include <cstdlib>
#include <pqxx/pqxx>

using namespace std;

class Funcionario
{
private:
    int matricula;
    string nome;
    double salario;

public:
    Funcionario()
    {
        matricula = 0;
        nome = "";
        salario = 0;
    }

    Funcionario(int matricula, string nome, double salario)
    {
        this->matricula = matricula;
        this->nome = nome;
        this->salario = salario;
    }

    int getMatricula()
    {
        return matricula;
    }

    string getNome()
    {
        return nome;
    }

    double getSalario()
    {
        return salario;
    }
};

class FuncionarioDAO
{
private:
    pqxx::connection con;

public:
    FuncionarioDAO(const string& url)
        : con(url)
    {
        cout << "\nConectado ao Railway com sucesso!\n";
    }

    void inserir(Funcionario f)
    {
        pqxx::work tx(con);

        tx.exec_params(
            "INSERT INTO funcionario "
            "(matricula, nome, salario) "
            "VALUES ($1, $2, $3)",
            f.getMatricula(),
            f.getNome(),
            f.getSalario()
        );

        tx.commit();

        cout << "\nFuncionario inserido.\n";
    }

    void listar()
    {
        pqxx::work tx(con);

        auto res = tx.exec(
            "SELECT * FROM funcionario "
            "ORDER BY matricula"
        );

        cout << "\n=== FUNCIONARIOS ===\n";

        for (auto row : res)
        {
            cout
                << "Matricula: "
                << row["matricula"].as<int>()

                << "\nNome: "
                << row["nome"].as<string>()

                << "\nSalario: R$ "
                << row["salario"].as<double>()

                << "\n--------------------\n";
        }
    }

    Funcionario buscar(int matricula)
    {
        pqxx::work tx(con);

        auto res = tx.exec_params(
            "SELECT * FROM funcionario "
            "WHERE matricula = $1",
            matricula
        );

        if (!res.empty())
        {
            return Funcionario(
                res[0]["matricula"].as<int>(),
                res[0]["nome"].as<string>(),
                res[0]["salario"].as<double>()
            );
        }

        return Funcionario();
    }

    void atualizarSalario(int matricula, double salario)
    {
        pqxx::work tx(con);

        auto res = tx.exec_params(
            "UPDATE funcionario "
            "SET salario = $1 "
            "WHERE matricula = $2",
            salario,
            matricula
        );

        tx.commit();

        if (res.affected_rows() > 0)
            cout << "\nSalario atualizado.\n";
        else
            cout << "\nFuncionario nao encontrado.\n";
    }

    void excluir(int matricula)
    {
        pqxx::work tx(con);

        auto res = tx.exec_params(
            "DELETE FROM funcionario "
            "WHERE matricula = $1",
            matricula
        );

        tx.commit();

        if (res.affected_rows() > 0)
            cout << "\nFuncionario removido.\n";
        else
            cout << "\nFuncionario nao encontrado.\n";
    }
};

int main()
{
    try
    {
        const char* url = getenv("DATABASE_URL");

        if (url == nullptr)
        {
            cout << "DATABASE_URL nao configurada!\n";
            return 1;
        }

        FuncionarioDAO dao(url);

        int opcao;

        do
        {
            cout << "\n";
            cout << "1 - Inserir\n";
            cout << "2 - Listar\n";
            cout << "3 - Buscar\n";
            cout << "4 - Atualizar Salario\n";
            cout << "5 - Excluir\n";
            cout << "0 - Sair\n";
            cout << "Opcao: ";

            cin >> opcao;

            switch (opcao)
            {
                case 1:
                {
                    int matricula;
                    string nome;
                    double salario;

                    cout << "Matricula: ";
                    cin >> matricula;

                    cin.ignore();

                    cout << "Nome: ";
                    getline(cin, nome);

                    cout << "Salario: ";
                    cin >> salario;

                    dao.inserir(
                        Funcionario(
                            matricula,
                            nome,
                            salario
                        )
                    );

                    break;
                }

                case 2:
                    dao.listar();
                    break;

                case 3:
                {
                    int matricula;

                    cout << "Matricula: ";
                    cin >> matricula;

                    Funcionario f =
                        dao.buscar(matricula);

                    if (f.getMatricula() != 0)
                    {
                        cout
                            << "\nMatricula: "
                            << f.getMatricula()

                            << "\nNome: "
                            << f.getNome()

                            << "\nSalario: "
                            << f.getSalario()
                            << endl;
                    }
                    else
                    {
                        cout
                            << "\nFuncionario nao encontrado.\n";
                    }

                    break;
                }

                case 4:
                {
                    int matricula;
                    double salario;

                    cout << "Matricula: ";
                    cin >> matricula;

                    cout << "Novo salario: ";
                    cin >> salario;

                    dao.atualizarSalario(
                        matricula,
                        salario
                    );

                    break;
                }

                case 5:
                {
                    int matricula;

                    cout << "Matricula: ";
                    cin >> matricula;

                    dao.excluir(matricula);

                    break;
                }

                case 0:
                    cout << "\nEncerrando...\n";
                    break;

                default:
                    cout << "\nOpcao invalida!\n";
            }

        } while (opcao != 0);
    }
    catch (const exception& e)
    {
        cerr << "\nErro:\n"
             << e.what()
             << endl;
    }

    return 0;
}
