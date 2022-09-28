package cz.cvut.fel.pjv;

public class NodeImpl implements Node {
    public Node left;
    public Node right;
    public int value;

    public NodeImpl (int value) {
        this.left = null;
        this.right = null;
        this.value = value;
    }

    public Node getLeft() {
        return left;
    }

    public Node getRight() {
        return right;
    }

    public int getValue() {
        return value;
    }
}
