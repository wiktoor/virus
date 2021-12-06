#ifdef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <vector>

template<typename Virus>
class VirusGenealogy {
    private:
        class Node {
            private:
                Virus virus;
                std::vector<std::shared_ptr<Node>> children;
                std::vector<std::weak_ptr<Node>> parents;
        }
        
}

#endif /* VIRUS_GENEALOGY_H */