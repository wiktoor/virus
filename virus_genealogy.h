#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <exception>
#include <iostream> // do usunięcia

struct VirusAlreadyCreated : public std::exception {
    const char* what() const throw() {
        return "VirusAlreadyCreated";
    }
};

struct VirusNotFound : public std::exception {
    const char* what() const throw() {
        return "VirusNotFound";
    }
};

struct TriedToRemoveStemVirus : public std::exception {
    const char* what() const throw() {
        return "TriedToRemoveStemVirus";
    }
};

template<typename Virus>
class VirusGenealogy {
private:

    Virus::id_type stem_id;

    using Virus_ptr_t = std::shared_ptr<Virus>;
    using Virus_ptrs_t = std::vector<Virus_ptr_t>;
    // Przydałyby się jeszcze usingi na Virus::id_type (bo typename'y)
    // i na std::vector<typename Virus::id_type>. Tylko używając ich
    // zmieniamy wygląd interfejsu. Na przykład get_parents zwraca
    // std::vector<typename Virus::id_type>, a jeśli zrobimy usinga,
    // to będziemy mieć inną nazwę zwracanego typu. Nie wiem, czy tak można.

    struct Node {

        // Tutaj typ zmienił się na pointer do wirusa. Możliwe, że wcześniej źle
        // źle korzystaliśmy z shared pointerów. Funkcja make_shared tworzy nowy
        // obiekt i zwraca wskaźnik do niego. Czyli nie powinniśmy jej używać,
        // gdy chcemy stworzyć kolejny wskaźnik do już istniejącego obiektu,
        // bo stworzyłaby nam nowy, identyczny obiekt. A przynajmniej w teorii
        // tak mogłoby się stać, bo tego nie wykryliśmy. Tak czy siak, tak jak teraz
        // mamy na pewno jest dobrze. Wywołujemy make_shared raz w konstruktorze
        // Noda, a jak potem chcemy nowy wskaźnik do istniejącego wirusa, to
        // kopiujemy wskaźnik virus_ptr.
        Virus_ptr_t virus_ptr = nullptr;
        std::vector<typename Virus::id_type> parents;
        Virus_ptrs_t children;

        // Jeśli make_shared lub konstruktor wirusa rzuci wyjątek, virus_ptr będzie
        // równy nullptr. Nie utworzymy wirusa, wszystko jest ok.
        Node(const Virus::id_type &id) : virus_ptr(std::make_shared<Virus>(id)) {}

        // funkcje do testowania
        void printNode() {
            std::cout << "Jestem wierzchołkiem: " << virus_ptr->get_id() << std::endl;
            std::cout << "Moi rodzice to: " << std::endl;
            for (auto parent : parents) {
                std::cout << parent << " ";
            }
            std::cout << std::endl << "Moi synowie to: " << std::endl;
            for (auto ptr : children) {
                std::cout << ptr->get_id() << " ";
            }
            std::cout << std::endl << std::endl;
        }

    };

    using Nodes_map_t = std::map<typename Virus::id_type, Node>;

    Nodes_map_t nodes;

public:

    class children_iterator {
    private:

        std::shared_ptr<Virus_ptrs_t> vec;
        size_t position;

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Virus;
        using difference_type = std::ptrdiff_t;
        using pointer = std::shared_ptr<Virus>;
        using reference = const Virus &;

        children_iterator() {}

        children_iterator(const Virus_ptrs_t &vec, size_t position) :
            vec(std::make_shared<Virus_ptrs_t>(vec)), position(position) {}

        children_iterator& operator=(const children_iterator &it) {
            position = it.position;
            vec = it.vec;
            return *this;
        }

        const Virus& operator*() const {
            return *(vec->at(position));
        }

        std::shared_ptr<Virus> operator->() {
            return vec->at(position);
        }

        children_iterator& operator++() {
            position++;
            return *this;
        }

        children_iterator operator++(int) {
            children_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        children_iterator& operator--() {
            position--;
            return *this;
        }

        children_iterator operator--(int) {
            children_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const children_iterator &a) const {
            return this->position == a.position && *(this->vec) == *(a.vec);
        }

        bool operator!=(const children_iterator &a) const {
            return !(*this == a);
        }

    };

    VirusGenealogy(const Virus::id_type &stem_id) : stem_id(stem_id) {
        nodes.emplace(stem_id, Node(stem_id));
    }

    Virus::id_type get_stem_id() const {
        return stem_id;
    }

    // Pozmieniać na iteratory w poniższych funkcjach, bo kilka odwołań do tego samego klucza.
    std::vector<typename Virus::id_type> get_parents(Virus::id_type const &id) const {
        if (!nodes.contains(id)) throw VirusNotFound();
        return nodes.at(id).parents;
    }

    bool exists(Virus::id_type const &id) const {
        return nodes.contains(id);
    }

    const Virus& operator[](typename Virus::id_type const &id) const {
        if (!nodes.contains(id))
            throw VirusNotFound();

        return *(nodes.at(id).virus_ptr);
    }

    VirusGenealogy<Virus>::children_iterator get_children_begin(Virus::id_type const &id) const {
        if (!nodes.contains(id))
            throw VirusNotFound();

        return children_iterator(nodes.at(id).children, 0);
    }

    VirusGenealogy<Virus>::children_iterator get_children_end(Virus::id_type const &id) const {
        if (!nodes.contains(id))
            throw VirusNotFound();

        return children_iterator(nodes.at(id).children, nodes.at(id).children.size());
    }

    void create(Virus::id_type const &id, Virus::id_type const &parent_id) {
        // Jeśli contains rzuci wyjątek, to nie będzie to miało żadnych skutków ubocznych.
        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        if (!nodes.contains(parent_id))
            throw VirusNotFound();

        // Poniżej wyjątki nie będą miały skutków ubocznych.
        Node node(id);
        node.parents.push_back(parent_id);
        auto parent_it = nodes.find(parent_id);
        // Tu jest przykład wrzucania kopii shared pointera.
        (parent_it->second).children.push_back(node.virus_ptr);

        try {
            nodes.emplace(id, node);
        }
        catch (...) {
            // pop_back() jest noexpept na niepustym vectorze
            (parent_it->second).children.pop_back();
            throw;
        }
    }

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids) {
        if (nodes.contains(id))
            throw VirusAlreadyCreated();
        for (typename Virus::id_type parent : parent_ids) {
            if (!nodes.contains(parent))
                throw VirusNotFound();
        }

        Node node(id);
        for (typename Virus::id_type parent_id : parent_ids) {
            node.parents.push_back(parent_id);
        }

        std::vector<typename Nodes_map_t::iterator> parents_it(parent_ids.size());
        size_t next_id = 0;
        try {
            for (; next_id < parent_ids.size(); next_id++) {
                parents_it[next_id] = nodes.find(parent_ids[next_id]);
                (parents_it[next_id]->second).children.push_back(node.virus_ptr);
            }
        } // Tu pewnie jest źle. Operator [] pewnie rzuca wyjątek. Chyba czeka nas działanie na kopii.
        catch (...) {
            for (size_t j = 0; j < next_id; j++) {
                // czy -> może rzucić wyjątek?
                (parents_it[j]->second).children.pop_back();
            }
            throw;
        }

        try {
            nodes.emplace(id, node);
        }
        catch (...) {
            for (size_t j = 0; j < parent_ids.size(); j++) {
                (parents_it[j]->second).children.pop_back();
            }
            throw;
        }
    }

    // Tu też iteratory, bo wielokrotne odwoływanie do kluczy child_ir i parent_id.
    void connect(Virus::id_type const &child_id, Virus::id_type const &parent_id) {
        if (!nodes.contains(child_id) || !nodes.contains(parent_id))
            throw VirusNotFound();

        std::vector<typename Virus::id_type>& parent_ids = nodes.at(child_id).parents;
        if (std::find(parent_ids.begin(), parent_ids.end(), parent_id) != parent_ids.end())
            return;

        parent_ids.push_back(parent_id);
        try {
            nodes.at(parent_id).children.push_back(nodes.at(child_id).virus_ptr);
        }
        catch (...) {
            parent_ids.pop_back();
            throw;
        }
    }

    // Zapowiada się świetna zabawa (:
    void remove(Virus::id_type const &id) {
        if (id == stem_id)
            throw TriedToRemoveStemVirus();
        if (!nodes.contains(id))
            throw VirusNotFound();

        Node& node = nodes.at(id);
        // usuwamy krawędzie od rodziców
        for (typename Virus::id_type parent_id : node.parents) {
            Node& parent = nodes.at(parent_id);
            for (size_t i = 0; i < parent.children.size(); i++) {
                if (parent.children[i]->get_id() == id) {
                    swap(parent.children[i], parent.children.back());
                    parent.children.pop_back();
                    break; // bo nie ma multikrawędzi
                }
            }
        }
        for (auto ptr : node.children) {
            Node& child = nodes.at(ptr->get_id());
            auto it = std::find(child.parents.begin(), child.parents.end(), id);
            child.parents.erase(it);
            if (!child.parents.size()) remove(child.virus_ptr->get_id());
        }
        nodes.erase(id);
    }

    // funkcje do testowania
    void printTree(Virus::id_type const &id) {
        Node& node = nodes.at(id);
        node.printNode();
        for (auto ptr : node.children) {
            printTree(ptr->get_id());
        }
    }

};

#endif /* VIRUS_GENEALOGY_H */