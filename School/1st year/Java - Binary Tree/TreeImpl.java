/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package cz.cvut.fel.pjv;

public class TreeImpl implements Tree {

    Node root;
    int[] values;

    public Node addNode(int start, int end) {
        if (start == end) return new NodeImpl(this.values[start]);
        else {
            int currentRoot = (end - start + 1) / 2 + start;
            NodeImpl n = new NodeImpl(this.values[currentRoot]);
            n.left = addNode(start, currentRoot - 1);
            if (currentRoot + 1 <= end) {
                n.right = addNode(currentRoot + 1, end);
            } else {
                n.right = null;
            }
            return n;
        }
    }

    public void setTree(int[] values) {
        this.values = values;
        this.root = (values.length > 0) ? addNode(0, values.length - 1) : null;
    }

    public Node getRoot() {
        return this.root;
    }

    public String toString() {
        String ret = "";
        if (this.root == null) return ret;
        else {
            ret += "- " + Integer.toString(this.root.getValue()) + "\n";
            if (this.root.getLeft() != null) ret += treeStr(this.root.getLeft(), 1);
            if (this.root.getRight() != null) ret += treeStr(this.root.getRight(), 1);
        }
        return ret;
    }

    private String treeStr(Node n, int counter) {
        String ret = "";
        for (int i = 0; i < counter; i++) {
            ret += " ";
        }
        ret += "- " + Integer.toString(n.getValue()) + "\n";
        if (n.getLeft() != null) {
            ret += treeStr(n.getLeft(), counter + 1);
        }
        if (n.getRight() != null){
            ret += treeStr(n.getRight(), counter + 1);
        }
        return ret;
    }
}