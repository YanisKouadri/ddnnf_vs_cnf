

#Represente un noeud du graph
#Possède un nom, une liste de noeuds qui attaquent ce noeud, une liste de noeuds attaqués par ce noeud
class Node:
    '''
    A class representing an argument (node) from an argumentation framework (graph).
    Each argument has a name (integer from .tgf format), an attacker's list (agressors) and an attacking list (victims). 
    '''
    
    def __init__(self,name):
        self.name=name
        self.attackers_list = []
        self.attacking_list = []
        
    #Redefinition of several operators for the Node object, lt is not used anymore but there's no harm in keeping it. 
    #(Its important to keep these redefinitions of repr, str and eq)
    def __repr__(self):
     return self.name
    def __str__(self):
     return self.name
    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self.name == other.name
        else:
            return self.name == other
    def __lt__(self, other):
        if isinstance(other, self.__class__):
            return int(self.name) < int(other.name)
        else:
            return int(self.name) < other
    


class Graph:
    '''
    A class representing the graph (argumentation framework), it has a list containing the nodes (arguments).
    '''
    def __init__(self):
        self.Nodes = []
        
    #The following is deprecated because its too computationally expensive, now that we're using tgf, it's useless
    """
    def get_node(self,nodeName):
        for element in self.Nodes:
            if(element.name == nodeName):
                return element
        return None 
    """